#!/bin/bash

# Detect OS and store in OS_TYPE
case "$(uname -s)" in
  Darwin*) OS_TYPE="macOS" ;;
  Linux*)  OS_TYPE="Linux" ;;
  CYGWIN*|MINGW*|MSYS*) OS_TYPE="Windows" ;;
  *)
    echo "Unsupported OS: $(uname -s)"
    pause_if_interactive
    exit 1
    ;;
esac

# Function to check if a command exists
function check_dependency {
    if ! command -v "$1" &> /dev/null; then
        echo "Error: Required dependency '$1' is not installed. Please install it before proceeding."
        pause_if_interactive
        exit 1
    fi
}

# Function to pause execution if running interactively
function pause_if_interactive {
    if [[ -t 1 ]]; then
        read -n 1 -s -r -p "Press any key to continue..."
        echo
    fi
}

# Function to check command success status
function check_command_success_status {
    "$@"
    local status=$?
    if [[ $status -ne 0 ]]; then
        echo "Error: Command '$*' failed."
        pause_if_interactive
        exit 1
    fi
}

# Function to download with curl or powershell
function download {
    local url="$1"
    local output="$2"

    if command -v curl &>/dev/null; then
        # echo "Downloading using curl..."
        check_command_success_status curl --fail --silent --show-error --retry 3 --connect-timeout 10 --max-time 60 -# -L "$url" -o "$output"

    elif command -v powershell &>/dev/null; then
        # echo "Downloading using PowerShell..."
        check_command_success_status powershell -Command "Invoke-WebRequest -Uri '$url' -OutFile '$output'"

    else
        echo "Error: curl is not available and no fallback download method found."
        pause_if_interactive
        exit 1
    fi
}

# Function to extract contents of zip file
function extract_zip {
    local zip_file="$1"
    local destination="$2"

    if command -v unzip &>/dev/null; then
        echo "Extracting using unzip..."
        check_command_success_status unzip -q "$zip_file" -d "$destination"

    elif [[ "$OS_TYPE" == "Windows" ]]; then
        if command -v powershell &>/dev/null; then
            # echo "Extracting using PowerShell..."
            check_command_success_status powershell -Command "Expand-Archive -Path '$zip_file' -DestinationPath '$destination' -Force"

        elif command -v 7z &>/dev/null; then
            # echo "Extracting using 7-Zip..."
            check_command_success_status 7z x "$zip_file" -o"$destination" -y

        else
            echo "Error: No suitable extraction tool found (unzip, PowerShell, or 7-Zip)."
            pause_if_interactive
            exit 1
        fi

    else
        echo "Error: unzip is required but not found."
        pause_if_interactive
        exit 1
    fi
}


# Set Globals
BUILD_TYPE="Release"
LOGGING_DIR="$(pwd)/logs"
THIRDPARTY_DIR="$(pwd)/src/thirdparty"
PROJECT_DIR="$(pwd)/game"
IMGUI_VERSION="v1.91.2"

# ------------------------------------------------------------------ #
# Ensure necessary directories, exist
check_command_success_status mkdir -p "$LOGGING_DIR" "$THIRDPARTY_DIR" "$PROJECT_DIR"


# ------------------------------------------------------------------ #
# Function to check configure CMake configuration file
function configure_cmake_config_file
{
	local config_file="$1"

	if [[ ! -f "$config_file" ]]; then
		echo "Panda3D configuration file not found."

		# Prompt user to input Panda3D installation path
		echo "Please provide the path to Panda3D installation."
		read -r -p "Enter the full path to Panda3D (e.g., C:/Panda3D-1.10.15-x64 or /opt/panda3d): " panda3D_path

		# Normalize path (replace backslashes with forward slashes)
		panda3D_path="${panda3D_path//\\//}"

		# Check if the provided path exists
		if [[ ! -d "$panda3D_path" ]]; then
			echo "Error: The provided Panda3D path '$panda3D_path' does not exist. Exiting."
			pause_if_interactive
			exit 1
		fi

		# Create the CMake config file with the provided path
		echo -e "Creating CMake configuration file...\n"

    cat << EOF > "$config_file"
# Path to the root installation of Panda3D
set(PANDA3D_ROOT "$panda3D_path" CACHE STRING "Path to the Panda3D installation")

# Include and library directories
set(PANDA3D_INCLUDE_DIR "\${PANDA3D_ROOT}/include")
set(PANDA3D_LIBRARY_DIR "\${PANDA3D_ROOT}/lib")
EOF
	fi
}

check_dependency "cmake" # Ensure cmake exists
configure_cmake_config_file "config.cmake"  # Create the CMake config file

# ------------------------------------------------------------------ #
# Download and extract ImGui if not already present
if [[ ! -d "$THIRDPARTY_DIR/imgui" ]]; then
    echo "Downloading ImGui ($IMGUI_VERSION)..."

    # Download
	url="https://github.com/ocornut/imgui/archive/refs/tags/$IMGUI_VERSION.zip"
	download "$url" "imgui.zip"

	# Extract
    echo "Extracting ImGui..."
	extract_zip "imgui.zip" "$THIRDPARTY_DIR"

    # Find extracted directory dynamically
    extracted_dir=$(find "$THIRDPARTY_DIR" -maxdepth 1 -type d -name "imgui-*" | head -n 1)
    if [[ -d "$extracted_dir" ]]; then
        check_command_success_status mv "$extracted_dir" "$THIRDPARTY_DIR/imgui"
    else
        echo "Error: ImGui extraction failed."
        pause_if_interactive
        exit 1
    fi
	
	# Remove unnecessary files
    echo "Cleaning up..."
    check_command_success_status rm imgui.zip

    echo -e "ImGui setup completed successfully.\n"
fi


# ------------------------------------------------------------------ #
# Function to display folder structure
declare -A USERS_PROJECTS
declare -A DEMO_PROJECTS

create_print_project_tree() 
{
    local base_dir="$(pwd)"
    
    # Define annotations for the top-level directories
    declare -A annotations=(
        ["game"]="  # Main directory for user-defined projects"
        ["demos"]=" # Directory for demo projects"
        ["builds"]="# Directory for build output"
    )

    echo -e "PandaEditor Project Configuration and Build System\n"
    echo "src"
    
    local total_projects_count=0
    
    # Loop through specific directories (game, demos, builds) inside src
    for top_level_dir in "game" "demos" "builds"; do
        local dir_path="$base_dir/$top_level_dir"

        # Check if the directory exists
        if [[ -d "$dir_path" ]]; then
            echo "├── $top_level_dir ${annotations[$top_level_dir]}"
            
            # Get a list of subdirectories (only top-level folders)
            local sub_dirs=("$dir_path"/*/)
            local total_subdirs=${#sub_dirs[@]}

            # Initialize an empty list to store project names for later use
            local count=0
            for sub_dir in "${sub_dirs[@]}"; do
                # Check if the directory is valid
                if [[ -d "$sub_dir" ]]; then
                    local dir_name=$(basename "$sub_dir")
                    count=$((count + 1))

                    # Check if it's one of reserved dirs
                    if [[ "$dir_name" == "_assets" ]]; then
                        continue
                    fi

                    # Collect user and demo projects
                    total_projects_count=$((total_projects_count+1))
                    if [[ "$top_level_dir" == "game" ]]; then
                        USERS_PROJECTS[$total_projects_count]="$dir_name"
                    elif [[ "$top_level_dir" == "demos" ]]; then
                        DEMO_PROJECTS[$total_projects_count]="$dir_name"
                    fi

                    # Print subdirectory with proper formatting
                    if [[ "$top_level_dir" == "game" ]] || [[ "$top_level_dir" == "demos" ]]; then
                        if [[ "$count" -eq "$total_subdirs" ]]; then
                            echo "│   └── $total_projects_count. $dir_name"  # Last item with └──
                        else
                            echo "│   ├── $total_projects_count. $dir_name"  # Other items with ├──
                        fi
                    else
                        if [[ "$count" -eq "$total_subdirs" ]]; then
                            echo "│   └── $count. $dir_name"  # Last item with └──
                        else
                            echo "│   ├── $count. $dir_name"  # Other items with ├──
                        fi
                    fi
                    
                fi
            done

            # Add extra line for 'game' and 'demos' directories
            if [[ "$top_level_dir" == "game" ]] || [[ "$top_level_dir" == "demos" ]]; then
                echo "│"
            fi
        fi
    done
    
    echo ""
}

# Function to get user input
PROJECT_NAME="" # project name
PROJECT_PATH="" # project directory
BUILD_DIR=""    # build directory

function get_project_from_user
{
    while true; do
        # Prompt the user for the project name
        read -p "Enter project name / index or -1 to exit: " project_name

        # Validate input
        if [[ -z "$project_name" ]]; then
            echo -e "Project name cannot be empty. Please try again.\n"
            continue
        elif [[ "$project_name" == "-1" ]]; then
            # Handle exit condition first
            exit 0
        elif [[ "${project_name:0:1}" == "-" ]]; then
            echo -e "Invalid project name: Projects name must not start with '-'. Please try again.\n"
            continue
        elif [[ "$project_name" =~ [^a-zA-Z0-9_-] ]]; then
            echo -e "Invalid project name: Only letters, numbers, hyphens (-), and underscores (_) are allowed. Please try again.\n"
            continue
        elif [[ "$project_name" == "assets" || "$project_name" == "example_code" ]]; then
            # 'assets' is a reserved folder
            echo -e "Invalid project name, try again.\n"
            continue
        elif [[ "$project_name" =~ ^[0-9]+$ ]]; then
            # Handle purely numeric names
            if [[ -v USERS_PROJECTS[$project_name] ]]; then
                project_name="${USERS_PROJECTS[$project_name]}"
            elif [[ -v DEMO_PROJECTS[$project_name] ]]; then
                project_name="${DEMO_PROJECTS[$project_name]}"
            else
                echo -e "Invalid project name: Names cannot be purely numeric. Please try again.\n"
                continue
            fi
        fi

        # Check if the project directory already exists
        if [[ -d "$(pwd)/game/$project_name" ]]; then
            PROJECT_PATH="$(pwd)/game/$project_name"
            PROJECT_NAME="$project_name"
            break
        elif [[ -d "$(pwd)/demos/$project_name" ]]; then
            PROJECT_PATH="$(pwd)/demos/$project_name"
            PROJECT_NAME="$project_name"
            
            # check if assets folder exists
            if [[ ! -d "$(pwd)/demos/_assets" ]]; then
                # configure demo assets folder
                echo "Assets folder not found."
                while true; do
                    read -p "Download the 'assets' folder? (y/n): " response
                    case "$response" in
                        [yY])
                            echo "Downloading 'assets'..."
                            check_command_success_status curl -L "PLACE_HOLDER_WEB_ADDRESS" -o assets.zip
                            check_command_success_status mkdir -p "$assets_dir"
                            check_command_success_status unzip assets.zip -d "$assets_dir"
                            check_command_success_status rm assets.zip
                            echo "'assets' folder downloaded and extracted successfully."
                            ;;
                        [nN])
                            echo -e "Assets download canceled. Please choose another project.\n"
                            continue 2
                            ;;
                        *)
                            echo -e "Invalid input. Please enter 'y' or 'n'.\n"
                            continue 1
                            ;;
                    esac
                done
            fi
            break
        fi

        # Handle non-existent project
        echo "Project '$project_name' does not exist."
        while true; do
            read -p "Create project '$project_name'? (y/n, -1 to exit): " choice
            case "$choice" in
                [yY])
                    check_command_success_status mkdir -p "$(pwd)/game/$project_name"
                    PROJECT_PATH="$(pwd)/game/$project_name" # Set PROJECT_PATH to the created directory
                    PROJECT_NAME="$project_name"             # Set PROJECT_NAME to the new project
                    break 2 # Exit both loops after creation
                    ;;
                [nN])
                    echo -e "Please enter a different project name.\n"
                    break # Re-prompt for a project name
                    ;;
                "-1")
                    exit 0 # Exit the script
                    ;;
                *)
                    echo -e "Invalid input. Please enter 'y', 'n', or '-1'.\n"
                    ;;
            esac
        done
    done
}

# Function to configure project initialization
function configure_project
{
    if [[ -n "$1" ]] && [[ ! -d "$(pwd)/demos/$1" ]] && [[ ! -d "$(pwd)/game/$1" ]]; then
        echo "Specified project $1 not found!"
        read -n 1 -s -r -p "Press any key to continue..."
        exit 1
    fi
        
    # if the project directory is empty
    if [[ -z "$(find "$PROJECT_PATH" -mindepth 1 -maxdepth 1)" ]]; then
        # Create a new main.cpp file with boilerplate code
        cat <<EOF > "$PROJECT_PATH/main.cpp"
#include "demon.h"


class MyApp {
public:
    MyApp() { 
	
        /* your code goes here... */
    }
	
	void start() {
	
		Demon::get_instance().start();
	}
};


int main(int argc, char* argv[]) {
    MyApp app;
    app.start();
    return 0;
}
EOF
    fi
    
    echo -e "Starting project '$(basename "$PROJECT_PATH")' \n"
}

# Function to set the build directory based on project name
function set_build_directory {    
    
    if [[ -n "$1" ]]; then
        BUILD_DIR="$(pwd)/builds/$1"
    else
        BUILD_DIR="$(pwd)/builds/default-project"
    fi
    
    # Ensure the build directory exists
    mkdir -p "$BUILD_DIR"
}

# Function to log CMake output
function log_cmake_output {
    local log_file="$1"
    shift
    
    # Check if the directory is writable and the file is either writable or can be created
    if [[ ! -w "$(dirname "$log_file")" ]]; then
        echo "Error: Cannot write to the directory of log file '$log_file'."
        read -n 1 -s -r -p "Press any key to continue..."
        exit 1
    fi

    # If the file exists, check if it's writable
    if [[ -f "$log_file" && ! -w "$log_file" ]]; then
        echo "Error: Cannot write to log file '$log_file'."
        read -n 1 -s -r -p "Press any key to continue..."
        exit 1
    fi

    # Execute command and log output
    "$@" 2>&1 | tee -a "$log_file"
}

# Function to configure project with CMake in the determined build directory
function run_cmake_config {
    local cmake_arch_option=""
    local path=$(basename "$PROJECT_PATH")
    local project="-DPROJECT_PATH=$path"

    # Determine architecture options for Windows
    if [[ "$OS_TYPE" = "Windows" ]]; then
        cmake_arch_option="-A x64"
    fi

    # Clear previous log and configure the project
    : > "$LOGGING_DIR"/build-log.log
    echo -e "Starting Configuration with CMake..." >> "$LOGGING_DIR"/build-log.log
    log_cmake_output "$LOGGING_DIR"/build-log.log cmake -B"$BUILD_DIR" -S. $cmake_arch_option $project
}

# Function to build project with CMake
function run_cmake_build {
    # : > "$BUILD_DIR"/build-log.log
    echo -e "\n\nStarting Build..." >> "$LOGGING_DIR"/build-log.log
    log_cmake_output "$LOGGING_DIR"/build-log.log cmake --build "$BUILD_DIR" --config Release
}


# ------------------------------------------------------------------ #
# Starting point
create_print_project_tree
get_project_from_user
configure_project "$PROJECT_NAME"
set_build_directory "$PROJECT_NAME"

# Run CMake configuration and build
run_cmake_config "$PROJECT_NAME"
run_cmake_build


# ------------------------------------------------------------------ #
# Finally, start the executable
# Set executable name based on OS
EXECUTABLE_NAME="game"
if [[ "$OS_TYPE" = "Windows" ]]; then
    EXECUTABLE_NAME="game.exe"
fi

echo "Checking for executable in $BUILD_DIR/Release/$EXECUTABLE_NAME"

# Check if the executable exists in the expected directory
if [[ -f "$BUILD_DIR/Release/$EXECUTABLE_NAME" ]]; then
    echo "Starting the game executable..."
    "$BUILD_DIR/Release/$EXECUTABLE_NAME"
    read -n 1 -s -r -p "Press any key to continue..."
else
    echo "Error: Executable '$EXECUTABLE_NAME' not found in $BUILD_DIR."
    read -n 1 -s -r -p "Press any key to continue..."
    exit 1
fi

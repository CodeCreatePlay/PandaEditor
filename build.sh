#!/bin/bash

# Detect OS and store in OS_TYPE
case "$OSTYPE" in
  darwin*)       OS_TYPE="macOS" ;;
  linux*)        OS_TYPE="Linux" ;;
  msys*|cygwin*) OS_TYPE="Windows" ;;
  *)             
    echo "Unsupported OS: $OSTYPE"
    read -n 1 -s -r -p "Exiting... Press any key to continue."
    exit 1
    ;;
esac

set -e  # Exit immediately if any command fails

# Function to check if a command exists
function check_dependency {
    if ! command -v "$1" &> /dev/null; then
        echo "Error: Required dependency '$1' is not installed. Please install it before proceeding."
        read -n 1 -s
        exit 1
    fi
}

# Check required dependencies
check_dependency "cmake"
check_dependency "curl"
check_dependency "unzip"

# Function to check command success status
function check_command_success_status {
    "$@"
    local status=$?
    if [ $status -ne 0 ]; then
        echo "Error: Command '$*' failed."
        read -n 1 -s
        exit 1
    fi
}

# Create logs directory if it doesn't exist
check_command_success_status mkdir -p logs

# Set default build type and paths
BUILD_TYPE="Release"
THIRDPARTY_DIR="$(pwd)/src/thirdparty"
IMGUI_VERSION="v1.91.2"

# Ensure third-party directory exists
if [ ! -d "$THIRDPARTY_DIR" ]; then
    echo "Creating third-party directory..."
    check_command_success_status mkdir -p "$THIRDPARTY_DIR"
fi

# Ensure project directory exists
PROJECT_DIR="$(pwd)/game"
if [ ! -d "$PROJECT_DIR" ]; then
    echo "Creating project directory..."
    check_command_success_status mkdir -p "$PROJECT_DIR"
fi

# Download ImGui if not already downloaded
if [ ! -d "$THIRDPARTY_DIR/imgui" ]; then
    echo "Downloading ImGui from GitHub as ZIP..."
    check_command_success_status curl -L "https://github.com/ocornut/imgui/archive/refs/tags/$IMGUI_VERSION.zip" -o imgui.zip

    echo "Extracting ImGui..."
    check_command_success_status unzip imgui.zip -d "$THIRDPARTY_DIR"

    # echo "Renaming ImGui directory..."
    check_command_success_status mv "$THIRDPARTY_DIR/imgui-${IMGUI_VERSION#v}" "$THIRDPARTY_DIR/imgui"

    echo "Removing ImGui zip file..."
    rm imgui.zip || { echo "Error: Failed to remove imgui.zip"; exit 1; }

    echo "ImGui setup completed."
fi

# Function to display folder structure with custom annotations
print_project_tree() {

	local base_dir="$(pwd)"
	
	# Define annotations for the top-level directories
	declare -A annotations=(
		["game"]="  # Main directory for user-defined projects"
		["demos"]=" # Directory for demo projects"
		["builds"]="# Directory for build output"
	)

	echo -e "PandaEditor Project Configuration and Build System\n"
	echo "src"

	# Loop through specific directories (game, demos, builds) inside src
	for top_level_dir in "game" "demos" "builds"; do
		# Check if each specified directory exists in base_dir
		if [ -d "$base_dir/$top_level_dir" ]; then
			echo "├── $top_level_dir ${annotations[$top_level_dir]}"
			
			# Get a list of top-level folders in each directory
			local sub_dirs=("$base_dir/$top_level_dir"/*)
			local count=0
			local total_subdirs=${#sub_dirs[@]}
			
			# Loop through only the top-level folders in each directory
			for sub_dir in "${sub_dirs[@]}"; do

				if [ -d "$sub_dir" ]; then
					count=$((count + 1))

					# Check if it's the last item
					if [ "$count" -eq "$total_subdirs" ]; then
						echo "│   └── $(basename "$sub_dir")"  # Last item with └──
					else
						echo "│   ├── $(basename "$sub_dir")"  # Other items with ├──
					fi
				fi
			done
			
			if [ "$top_level_dir" == "game" ] || [ "$top_level_dir" == "demos" ] ; then
				echo "│"
			fi
			
		fi
	done
}

PROJECT_NAME="-1" # project name
PROJECT_PATH="-1" # project directory
BUILD_DIR="-1" # build directory

function get_project {
	# Prompt the user for the project name
	read -p "Enter the project name: " project_name

	# Check if the project directory does not exist
	if [ ! -z "$project_name" ] && { [ ! -d "$(pwd)/game/$project_name" ] && [ ! -d "$(pwd)/demos/$project_name" ]; }; then
		while true; do
			# Prompt user to create the project or exit
			read -p "Project '$project_name' does not exist. Create it now? (y/n, -1 to exit): " choice

			if [[ "$choice" == "y" || "$choice" == "Y" ]]; then
				# If yes, then create project
				echo "Creating project '$project_name'..."
				check_command_success_status mkdir -p "$(pwd)/game/$project_name"
				break  # Exit the loop after creating the project
			elif [[ "$choice" == "n" || "$choice" == "N" ]]; then
				clear
				print_project_tree
				echo ""
				get_project  # Call get_project again to prompt for a new project name
				break
			elif [[ "$choice" == "-1" ]]; then
				exit 1  # Exit the script
			else
				# If input is irrelevant, do nothing and re-prompt
				echo -e "-- Invalid input. Please enter 'y', 'n', or '-1'.\n"
			fi
		done
	fi

	PROJECT_NAME="$project_name"
}

function set_project_directory {    

	if [ -n "$1" ] && [ ! -d "$(pwd)/demos/$1" ] && [ ! -d "$(pwd)/game/$1" ]; then
		echo "Specified project $1 not found!"
		read -n 1 -s -r -p "Press any key to continue..."
		exit 1
	fi
	
	if [ -n "$1" ] && [ -d "$(pwd)/demos/$1" ]; then
		PROJECT_PATH="$(pwd)/demos/$1"
		configure_demo_project "$1"
	elif [ -n "$1" ] && [ -d "$(pwd)/game/$1" ]; then
		PROJECT_PATH="$(pwd)/game/$1"
		
		# if project directory is empty, as in the case when user has created a new project 
		if [ -z "$(ls -A "$PROJECT_PATH")" ]; then
			touch "$PROJECT_PATH"/main.cpp # create a new main file for some boilerplate code
			echo -e '#include "demon.h"\n\n'           	             > "$PROJECT_PATH"/main.cpp
			echo -e 'int main(int argc, char* argv[]) {\n'          >> "$PROJECT_PATH"/main.cpp
			echo -e '\tDemon demon;\n\tdemon.start();\n\treturn 0;' >> "$PROJECT_PATH"/main.cpp
			echo -e '}'   >> "$PROJECT_PATH"/main.cpp         	    >> "$PROJECT_PATH"/main.cpp
		fi
	fi
	
	local proj_name=$(basename "$PROJECT_PATH")
	echo "-- Starting project '$proj_name'"
}

# Function to set the build directory based on project name
function set_build_directory {    
	
	if [ -n "$1" ]; then
		BUILD_DIR="$(pwd)/builds/$1"
	else
		BUILD_DIR="$(pwd)/builds/default-project"
	fi
	
    # Ensure the build directory exists
    mkdir -p "$BUILD_DIR"
}

function configure_demo_project {

	# Check for demo project argument and handle assets
	ASSETS_DIR="$(pwd)/demos/assets"

	if [ $# -gt 0 ] && [ -d "$(pwd)/demos/$1" ]; then
		echo "-- Starting demo project: $1"
		if [ ! -d "$ASSETS_DIR" ]; then
			echo "Demo assets folder not found."
			read -p "Download the 'assets' folder? (y/n): " response
			if [[ "$response" =~ ^[Yy]$ ]]; then
				echo "Downloading 'assets'..."
				check_command_success_status curl -L "PLACE_HOLDER_WEB_ADDRESS" -o assets.zip
				check_command_success_status mkdir -p "$ASSETS_DIR"
				check_command_success_status unzip assets.zip -d "$ASSETS_DIR"
				check_command_success_status rm assets.zip
				echo "'assets' downloaded and extracted successfully."
			else
				echo "Assets download canceled."
				read -n 1 -s -r -p "Press any key to exit."
				exit 1
			fi
		fi
	fi
}

# Function to log CMake output
function log_cmake_output {
    local log_file="$1"
    shift
    if ! touch "$log_file" &> /dev/null; then
        echo "Error: Cannot write to log file '$log_file'."
        exit 1
    fi
    "$@" 2>&1 | tee -a "$log_file"
    check_command_success_status
}

# Configure project with CMake in the determined build directory
function run_cmake_config {
    local cmake_arch_option=""
	local path=$(basename "$PROJECT_PATH")
	local project="-DPROJECT_PATH=$path"

    # Determine architecture options for Windows
    if [ "$OS_TYPE" = "Windows" ]; then
        cmake_arch_option="-A x64"
    fi

    # Clear previous log and configure the project
    : > "$BUILD_DIR"/build-log.log
    echo -e "Starting Configuration with CMake..." >> "$BUILD_DIR"/build-log.log
    log_cmake_output "$BUILD_DIR"/build-log.log cmake -B"$BUILD_DIR" -S. $cmake_arch_option $project
}

# Build project with CMake
function run_cmake_build {
    # : > "$BUILD_DIR"/cmake_build_output.log
    echo -e "\n\nStarting Build..." >> "$BUILD_DIR"/build-log.log
    log_cmake_output "$BUILD_DIR"/build-log.log cmake --build "$BUILD_DIR" --config Release
}

# Starting point
print_project_tree
echo ""

get_project
set_project_directory "$PROJECT_NAME"
set_build_directory "$PROJECT_NAME"

# Run CMake configuration and build
run_cmake_config "$PROJECT_NAME"
run_cmake_build

# Start the executable
# Set executable name based on OS
EXECUTABLE_NAME="game"
if [ "$OS_TYPE" = "Windows" ]; then
    EXECUTABLE_NAME="game.exe"
fi

# Check if the executable exists in the expected directory
if [ -f "$BUILD_DIR/Release/$EXECUTABLE_NAME" ]; then
    echo "Starting the game executable..."
    "$BUILD_DIR/Release/$EXECUTABLE_NAME"
	read -n 1 -s -r -p "Press any key to continue..."
else
    echo "Error: Executable '$EXECUTABLE_NAME' not found in $BUILD_DIR."
    read -n 1 -s -r -p "Press any key to continue..."
    # exit 1
fi

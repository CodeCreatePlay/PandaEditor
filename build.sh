#!/bin/bash

# Detect OS and store in OS_TYPE
case "$OSTYPE" in
  darwin*)       OS_TYPE="macOS" ;;
  linux*)        OS_TYPE="Linux" ;;
  msys*|cygwin*) OS_TYPE="Windows" ;;
  *)             
    echo "Unsupported OS: $OSTYPE"
    echo "Exiting... Press any key to continue."
    read -n 1 -s  # Wait for a key press
    exit 1  # Exit the script with an error code
    ;;
esac

set -e  # Exit immediately if any command fails

# Function to check if a command exists
function check_dependency {
    if ! command -v "$1" &> /dev/null; then
        echo "Error: Required dependency '$1' is not installed. Please install it before proceeding."
        read -n 1 -s  # Wait for a key press
        exit 1
    fi
}

# Check required dependencies
check_dependency "cmake"
check_dependency "curl"
check_dependency "unzip"

# Function to check command success status
function check_command_success_status {

	# "$@" is a special variable that represents all the arguments passed to a 
	# script or a function, preserving each argument as a separate word. 
	# It is commonly used in functions # when you want to forward all the arguments
	# received by a function to another command or function.
	# On the other hand $* treats all arguments as a single word.

    "$@"
    local status=$?  # Store the exit status immediately
    if [ $status -ne 0 ]; then
        echo "Error: Command '$*' failed."
        read -n 1 -s  # Wait for a key press
        exit 1  # Exit the script with an error code
    fi
}

# Create logs directory if it doesn't exist
check_command_success_status mkdir -p logs

# Set default build type
BUILD_TYPE="Release"

# Check for demo project argument
DEMOS_DIR="$(pwd)/demos"

if [ $# -gt 0 ]; then
    echo "Starting demo project: $1"
    if [ ! -d "$DEMOS_DIR" ]; then
        echo "Error: Demos folder not found."
        read -n 1 -s  # Wait for a key press
        exit 1
    fi
fi

# Ensure project directory exists
PROJECT_DIR="$(pwd)/game"

if [ ! -d "$PROJECT_DIR" ]; then
    echo "Project directory not found, creating it now..."
    check_command_success_status mkdir -p "$PROJECT_DIR"
fi

# Set up variables for third-party dependencies and ImGui version
THIRDPARTY_DIR="$(pwd)/src/thirdparty"
IMGUI_VERSION="v1.91.2"
IMGUI_VERSION_="1.91.2"

# Ensure third-party directory exists
if [ ! -d "$THIRDPARTY_DIR" ]; then
    echo "Creating third-party directory..."
    check_command_success_status mkdir -p "$THIRDPARTY_DIR"
fi

# Download ImGui if not already downloaded
if [ ! -d "$THIRDPARTY_DIR/imgui" ]; then
    echo "Downloading ImGui from GitHub as ZIP..."
    check_command_success_status curl -L "https://github.com/ocornut/imgui/archive/refs/tags/$IMGUI_VERSION.zip" -o imgui.zip
	
    echo "Extracting ImGui..."
    check_command_success_status unzip imgui.zip -d "$THIRDPARTY_DIR"

    # Rename the extracted folder to imgui
    mv "$THIRDPARTY_DIR/imgui-$IMGUI_VERSION_" "$THIRDPARTY_DIR/imgui"
    rm imgui.zip  # Clean up the ZIP file after extraction
else
    echo "ImGui already downloaded."
fi

# Log CMake output function (logs stdout and stderr)
function log_cmake_output {
    local log_file="$1"
    shift  # Remove the first argument, which is the log file name
	
    # Check if log file is writable
    if ! touch "$log_file" &> /dev/null; then
        echo "Error: Cannot write to log file '$log_file'."
        exit 1
    fi
	
    # Log the command output
    "$@" | tee -a "$log_file"
}

# CMake configuration function (log stdout and stderr)
function run_cmake_config {
    local cmake_arch_option=""
    local demo_option="-DDEMO_PROJECT=-1"

    # Check if an argument was provided and set DEMO_PROJECT option
    if [ -n "$1" ]; then
        demo_option="-DDEMO_PROJECT=$1"
        echo "Configuring demo project: $1"
    fi
	
    # Set architecture option if on Windows
    if [ "$OS_TYPE" = "Windows" ]; then
        cmake_arch_option="-A x64"
    fi

    # Clear the log file before logging
    : > logs/cmake_config_output.log
	
    echo "Configuring the project with CMake..."
    log_cmake_output logs/cmake_config_output.log cmake -Bbuild -S. $cmake_arch_option $demo_option
}

# CMake build function (log stdout and stderr)
function run_cmake_build {
    # Clear the log file before logging
    : > logs/cmake_build_output.log

    echo "Building the project..."
    log_cmake_output logs/cmake_build_output.log cmake --build build --config Release
}

# Run CMake configuration and build
run_cmake_config "$1"
run_cmake_build

# Start the executable after building
EXECUTABLE_NAME="game"  # Default executable name

if [ "$OS_TYPE" = "Windows" ]; then
    EXECUTABLE_NAME="game.exe"  # Set executable name for Windows
fi

# Check if the executable exists
if [ -f "build/Release/$EXECUTABLE_NAME" ]; then
    echo "Starting the game executable..."
    ./build/Release/$EXECUTABLE_NAME  # Run the executable
else
    echo "Error: Executable '$EXECUTABLE_NAME' not found in 'build' directory."
    exit 1
fi

# Pause the script (for environments that require a key press to continue)
read -n 1 -s -r -p "Press any key to continue..."

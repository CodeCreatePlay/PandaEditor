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

# Download ImGui if not already downloaded
if [ ! -d "$THIRDPARTY_DIR/imgui" ]; then
    echo "Downloading ImGui from GitHub as ZIP..."
    check_command_success_status curl -L "https://github.com/ocornut/imgui/archive/refs/tags/$IMGUI_VERSION.zip" -o imgui.zip

    echo "Extracting ImGui..."
    check_command_success_status unzip imgui.zip -d "$THIRDPARTY_DIR"

    # Rename the extracted folder to imgui
    check_command_success_status mv "$THIRDPARTY_DIR/imgui-$IMGUI_VERSION" "$THIRDPARTY_DIR/imgui"
    check_command_success_status rm imgui.zip
else
    echo "ImGui already downloaded."
fi

# Ensure project directory exists
PROJECT_DIR="$(pwd)/game"
if [ ! -d "$PROJECT_DIR" ]; then
    echo "Creating project directory..."
    check_command_success_status mkdir -p "$PROJECT_DIR"
fi

# Check for demo project argument and handle assets
ASSETS_DIR="$(pwd)/demos/assets"
if [ $# -gt 0 ]; then
    echo "Starting demo project: $1"
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

# Configure project with CMake
function run_cmake_config {
    local cmake_arch_option=""
    local demo_option="-DDEMO_PROJECT=-1"
    if [ -n "$1" ]; then
        demo_option="-DDEMO_PROJECT=$1"
        echo "Configuring demo project: $1"
    fi
    if [ "$OS_TYPE" = "Windows" ]; then
        cmake_arch_option="-A x64"
    fi
    : > logs/cmake_config_output.log
    echo "Configuring the project with CMake..."
    log_cmake_output logs/cmake_config_output.log cmake -Bbuild -S. $cmake_arch_option $demo_option
}

# Build project with CMake
function run_cmake_build {
    : > logs/cmake_build_output.log
    echo "Building the project..."
    log_cmake_output logs/cmake_build_output.log cmake --build build --config Release
}

# Run CMake configuration and build
run_cmake_config "$1"
run_cmake_build

# Start the executable
EXECUTABLE_NAME="game"
[ "$OS_TYPE" = "Windows" ] && EXECUTABLE_NAME="game.exe"
if [ -f "build/Release/$EXECUTABLE_NAME" ]; then
    echo "Starting the game executable..."
    ./build/Release/$EXECUTABLE_NAME
else
    echo "Error: Executable '$EXECUTABLE_NAME' not found."
    exit 1
fi

# Pause script (optional)
read -n 1 -s -r -p "Press any key to continue..."

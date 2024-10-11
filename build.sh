#!/bin/bash

# Set to exit immediately if any command fails
set -e

# Create logs directory if it doesn't exist
mkdir -p logs

# Ensure project directory exists
PROJECT_DIR="$(pwd)/game"

if [ ! -d "$PROJECT_DIR" ]; then
    echo "Project directory not found, creating it now..."
    mkdir -p "$PROJECT_DIR"
fi

# Set up variables for third-party dependencies and ImGui version
THIRDPARTY_DIR="$(pwd)/src/thirdparty"
IMGUI_VERSION="v1.91.2"
IMGUI_VERSION_="1.91.2"

# Ensure third-party directory exists
if [ ! -d "$THIRDPARTY_DIR" ]; then
    echo "Creating third-party directory..."
    mkdir -p "$THIRDPARTY_DIR"
fi

# Download ImGui as a ZIP if not already downloaded
if [ ! -d "$THIRDPARTY_DIR/imgui" ]; then
    echo "Downloading ImGui from GitHub as ZIP..."
    curl -L "https://github.com/ocornut/imgui/archive/refs/tags/$IMGUI_VERSION.zip" -o imgui.zip

    # Extract the ZIP file into the third-party directory
    echo "Extracting ImGui..."
    unzip imgui.zip -d "$THIRDPARTY_DIR"

    # Rename the extracted folder to imgui
    mv "$THIRDPARTY_DIR/imgui-$IMGUI_VERSION_" "$THIRDPARTY_DIR/imgui"

    # Clean up the ZIP file after extraction
    rm imgui.zip
else
    echo "ImGui already downloaded."
fi

# CMake configuration function (log stdout and stderr)
function run_cmake_config {
    echo "Configuring the project with CMake..."
    # cmake -Bbuild -S. -A x64 
	cmake -Bbuild -S. -A x64 \
		> >(tee logs/cmake_config_output.log) \
		2>&1 | tee -a logs/cmake_config_output.log
}

# CMake build function (log stdout and stderr)
function run_cmake_build {
    echo "Building the project..."
    # cmake --build build --config Release
	cmake --build build --config Release \
		> >(tee logs/cmake_build_output.log) \
		2>&1 | tee -a logs/cmake_build_output.log
}

# Run CMake configuration and build
run_cmake_config
run_cmake_build

# Pause the script (for environments that require a key press to continue)
read -n 1 -s -r -p "Press any key to continue..."

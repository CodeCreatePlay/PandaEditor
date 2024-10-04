#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

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

    # Check if the download was successful
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to download ImGui ZIP."
        exit 1
    fi

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

# Run CMake to configure the project
echo "Configuring the project with CMake..."
cmake -Bbuild -S. -A x64

# Check if CMake configuration succeeded
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    exit 1
fi

# Build the project
echo "Building the project..."
cmake --build build --config Release

# Check if the build succeeded
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed."
    exit 1
fi

# Pause the script (for environments that require a key press to continue)
read -n 1 -s -r -p "Press any key to continue..."
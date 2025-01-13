#!/bin/bash

# Paths and Directories
MinGW_Path="/c/msys64/usr/bin"
export PATH="$MinGW_Path:$PATH"

# Global variables
INCLUDE_DIRS=("../src/include")
FILE_NAME=""
OUTPUT=""

# Generate include flags
INCLUDE_FLAGS=""
for dir in "${INCLUDE_DIRS[@]}"; do
    INCLUDE_FLAGS+="-I$dir "
done

# Get user input
function get_program() {
	while true; do
		read -p "Enter name of program to build or -1 to exit: " program_name
		
		if [[ "$program_name" == "-1" ]]; then
			exit 1
		elif [[ -f "$(pwd)/example_code/$program_name.cpp" ]]; then
			OUTPUT="$(pwd)/example_code/$program_name"
			FILE_NAME="$(pwd)/example_code/$program_name.cpp"
			break
		elif [[ -f "$(pwd)/example_code/$program_name.hpp" ]]; then
			OUTPUT="$(pwd)/example_code/$program_name"
			FILE_NAME="$(pwd)/example_code/$program_name.hpp"
			break
		else
			echo -e "$(pwd)/example_code/$program_name\n"
			echo -e "Invalid entry! \n"
			continue
		fi
	done
}

# 
while true; do
	get_program

	# Compile with g++
	g++ $INCLUDE_FLAGS "$FILE_NAME" -o "$OUTPUT"

	# Check compilation success
	if [ $? -ne 0 ]; then
		echo "Build failed!"
		read -n 1 -s -r -p "press any key to exit..."
		exit 1
	fi

	echo "Build successful! Output: $OUTPUT"

	# Determine OS and set executable name
	OS_TYPE=$(uname)
	EXECUTABLE_NAME="$OUTPUT"
	if [[ "$OS_TYPE" == "Windows"* ]]; then
		EXECUTABLE_NAME="$OUTPUT.exe"
	fi

	# Run the executable
	echo -e "Starting the executable...\n"
	"$EXECUTABLE_NAME"

	# Blank space
	echo ""

	# Ask if the user wants to restart
	read -p "Do you want to build and run another program? (y/n): " restart_choice
	if [[ "$restart_choice" != "y" ]]; then
		echo "Exiting..."
		exit 0
	fi
done

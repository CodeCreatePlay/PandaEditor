# CMake minimum version and project declaration
cmake_minimum_required(VERSION 3.14)
project(PandaEditor LANGUAGES CXX)

# Check if CMake is using the Visual Studio generator
if(NOT "${CMAKE_GENERATOR}" MATCHES "Visual Studio")
    message(FATAL_ERROR "Visual Studio not found. CMake requires Visual Studio for this project.")
endif()

# Include the FetchContent module to use FetchContent_Declare and FetchContent_MakeAvailable
include(FetchContent)

# Define the path to the thirdparty directory
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/src/thirdparty)

# Ensure the thirdparty folder exists before we proceed with anything else
# file(MAKE_DIRECTORY ${THIRDPARTY_DIR})

# Define the path to the source directory (the folder containing .cpp files)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
#
#
# ______________________________ PandaEditor setup ______________________________
# Automatically add all .cpp files from the source directory and subdirectories
file(GLOB_RECURSE DEV_SOURCES ${SOURCE_DIR}/*.cpp)

# Remove any files in the thirdparty directory from the source list
list(FILTER DEV_SOURCES EXCLUDE REGEX "${THIRDPARTY_DIR}/.*")

# Manually add main.cpp
set(SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)

# Combine main.cpp with the files from the src folder
list(APPEND SOURCES ${DEV_SOURCES})

# Print out the sources for debugging (optional)
# message(STATUS "Source files: ${SOURCES}")

# Add the executable, passing the collected .cpp files
add_executable(game ${SOURCES})

target_include_directories(game PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
#
#
# ______________________________ Panda3D setup ______________________________
# Load the config file if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/config.cmake")
    include("${CMAKE_SOURCE_DIR}/config.cmake")
else()
    message(FATAL_ERROR "Config file not found: ${CMAKE_SOURCE_DIR}/config.cmake. Please create it.")
endif()

# Set the paths to Panda3D include and lib directories based on the config file
set(PANDA3D_INCLUDE_DIR "${PANDA3D_ROOT}/include")
set(PANDA3D_LIBRARY_DIR "${PANDA3D_ROOT}/lib")

# Include the Panda3D headers
target_include_directories(game PRIVATE ${PANDA3D_INCLUDE_DIR})

# Link the Panda3D libraries
target_link_directories(game PRIVATE ${PANDA3D_LIBRARY_DIR})

# Specify the Panda3D libraries to link with
target_link_libraries(game
    libp3framework.lib
    libpanda.lib
    libpandaexpress.lib
    libp3dtool.lib
    libp3dtoolconfig.lib
)
#
#
# ______________________________ imgui setup ______________________________
# Define the path to the ImGui directory (since it's downloaded via batch script)
set(IMGUI_DIR ${THIRDPARTY_DIR}/imgui)

# Include the ImGui headers and link the ImGui sources
if(EXISTS ${IMGUI_DIR})
    message(STATUS "ImGui found, linking with the project.")
    target_include_directories(game PRIVATE ${IMGUI_DIR})
    
    # Link the ImGui source files manually
    target_sources(game PRIVATE
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
    )
else()
    message(FATAL_ERROR "ImGui not found. Ensure it is downloaded to the correct location.")
endif()

# Set C++ Standard (optional, if needed)
set_target_properties(game PROPERTIES CXX_STANDARD 98)

# Add a custom command to run the executable after the build is complete
add_custom_command(TARGET game
    POST_BUILD
    COMMAND $<TARGET_FILE:game>  # Runs the game executable after build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}  # Optional: Set working directory
    COMMENT "Running game.exe after build..."
)
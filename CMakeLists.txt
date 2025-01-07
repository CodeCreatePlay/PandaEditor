cmake_minimum_required(VERSION 3.14)
project(PandaEditor LANGUAGES CXX)

# Check for the appropriate compiler
if(WIN32)
    if(MSVC)
        message(STATUS "Using Microsoft Visual Studio compiler on Windows.")
    else()
        message(FATAL_ERROR "This project requires Microsoft Visual Studio compiler on Windows. Please install Visual Studio with C++ build tools.")
    endif()
else()
    message(STATUS "Auto-detecting compiler (GCC or Clang).")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "GCC compiler detected.")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Clang compiler detected.")
    else()
        message(WARNING "Unsupported compiler detected. PandaEditor requires GCC or Clang.")
    endif()
endif()

# 
set(CMAKE_CXX_STANDARD 14)        	 # Use C++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Ensure the standard is supported
set(CMAKE_CXX_EXTENSIONS OFF)        # Disable compiler-specific extensions

# ---------------- PANDA_EDITOR-SETUP ---------------- #
# Directory Paths
set(THIRDPARTY_DIR_SRC ${CMAKE_SOURCE_DIR}/src/thirdparty)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Select the main script and project directory based on PROJECT_PATH variable
if(PROJECT_PATH AND NOT "${PROJECT_PATH}" STREQUAL "-1")
    if(EXISTS ${CMAKE_SOURCE_DIR}/demos/${PROJECT_PATH})
        set(MAIN_SCRIPT ${CMAKE_SOURCE_DIR}/demos/${PROJECT_PATH}/main.cpp)
        set(GAME_DIR ${CMAKE_SOURCE_DIR}/demos/${PROJECT_PATH})
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/game/${PROJECT_PATH})
        set(MAIN_SCRIPT ${CMAKE_SOURCE_DIR}/game/${PROJECT_PATH}/main.cpp)
        set(GAME_DIR ${CMAKE_SOURCE_DIR}/game/${PROJECT_PATH})
    else()
        message(FATAL_ERROR "Specified project '${PROJECT_PATH}' not found in demos or game directories.")
    endif()

    # Remove the main.cpp from the list of sources in GAME_DIR to avoid duplication
	file(GLOB_RECURSE GAME_SOURCES ${GAME_DIR}/*.cpp)
	list(REMOVE_ITEM GAME_SOURCES ${MAIN_SCRIPT})  # Remove the main.cpp that we already set
endif()

# Gather source files, excluding third-party directories and main.cpp
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX "${THIRDPARTY_DIR_SRC}/.*")

# Set up the source list for the executable, ensuring only one main.cpp is included
set(SOURCES ${MAIN_SCRIPT} ${SOURCE_FILES} ${GAME_SOURCES})

# Add the executable target
add_executable(game ${SOURCES})

# Include directories for the target
target_include_directories(game PRIVATE 
    ${SOURCE_DIR}/include 
    ${SOURCE_DIR}/utils/include 
	${SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/game/include
)

# ---------------- PANDA3D-SETUP ---------------- #
# Load Panda3D configuration
if(EXISTS "${CMAKE_SOURCE_DIR}/config.cmake")
    include("${CMAKE_SOURCE_DIR}/config.cmake")
else()
    message(FATAL_ERROR "Config file not found: ${CMAKE_SOURCE_DIR}/config.cmake. Please create it.")
endif()

# Find and link Panda3D libraries
find_library(PANDA_FRAMEWORK libp3framework PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDA_LIB libpanda PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDAEXPRESS_LIB libpandaexpress PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOL_LIB libp3dtool PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOLCONFIG_LIB libp3dtoolconfig PATHS ${PANDA3D_LIBRARY_DIR})

if(NOT PANDA_FRAMEWORK OR NOT PANDA_LIB OR NOT PANDAEXPRESS_LIB OR NOT DTOOL_LIB OR NOT DTOOLCONFIG_LIB)
    message(FATAL_ERROR "One or more Panda3D libraries were not found. Please check your installation.")
endif()

# ---------------- IMGUI-SETUP ---------------- #
# ImGui setup
set(IMGUI_DIR ${THIRDPARTY_DIR_SRC}/imgui)
if(EXISTS ${IMGUI_DIR})
    message(STATUS "ImGui found, linking with the project.")
    target_include_directories(game PRIVATE ${IMGUI_DIR})
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

# ---------------- BUILD-SETUP ---------------- #
# Force recompilation of the main script when rebuilding
add_custom_target(
    force_recompile_main ALL
    COMMAND ${CMAKE_COMMAND} -E touch ${MAIN_SCRIPT}
    COMMENT "Force recompilation."
)
add_dependencies(game force_recompile_main)

# Link Panda3D libraries
target_include_directories(game PRIVATE ${PANDA3D_INCLUDE_DIR})
target_link_libraries(game PRIVATE ${PANDA_FRAMEWORK} ${PANDA_LIB} ${PANDAEXPRESS_LIB} ${DTOOL_LIB} ${DTOOLCONFIG_LIB})

# Set C++ standard if needed
# set_target_properties(game PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# ---------------- POST_BUILD-SETUP ---------------- #
# Optional: Add post-build command to run the game executable
if(FALSE)  # Change to true to enable
    if(WIN32)
        add_custom_command(TARGET game
            POST_BUILD
            COMMAND $<TARGET_FILE:game>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running game.exe on Windows..."
        )
    elseif(UNIX)
        add_custom_command(TARGET game
            POST_BUILD
            COMMAND $<TARGET_FILE:game>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running game on Unix-like platform..."
        )
    endif()
endif()

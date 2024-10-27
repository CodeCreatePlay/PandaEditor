cmake_minimum_required(VERSION 3.14)
project(PandaEditor LANGUAGES CXX)

# Check for the appropriate compiler
if(WIN32)
    # Ensure that Microsoft Visual Studio (MSVC) is used
    if(MSVC)
        message(STATUS "Using Microsoft Visual Studio compiler on Windows.")
    else()
        message(FATAL_ERROR "This project requires Microsoft Visual Studio compiler on Windows. Please install Visual Studio with C++ build tools.")
    endif()
else()
    # Auto-detect the compiler for non-Windows platforms
    message(STATUS "Auto-detecting compiler (GCC or Clang).")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "GCC compiler detected.")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Clang compiler detected.")
    else()
        message(WARNING "Unsupported compiler detected. PandaEditor requires GCC or Clang.")
    endif()
endif()

# Directory Paths
set(THIRDPARTY_DIR_SRC ${CMAKE_SOURCE_DIR}/src/thirdparty)
set(THIRDPARTY_DIR_GAME ${CMAKE_SOURCE_DIR}/game/thirdparty)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(GAME_DIR ${CMAKE_SOURCE_DIR}/game)

# Main source file selection based on DEMO_PROJECT variable
if(DEMO_PROJECT AND NOT "${DEMO_PROJECT}" STREQUAL "-1")
    set(MAIN_SCRIPT ${CMAKE_SOURCE_DIR}/demos/${DEMO_PROJECT}/main.cpp)
else()
    set(MAIN_SCRIPT ${CMAKE_SOURCE_DIR}/main.cpp)
endif()

# Gather source files from development and game directories
file(GLOB_RECURSE DEV_SOURCES ${SOURCE_DIR}/*.cpp)
list(FILTER DEV_SOURCES EXCLUDE REGEX "${THIRDPARTY_DIR_SRC}/.*")
file(GLOB_RECURSE GAME_SOURCES ${GAME_DIR}/*.cpp)
list(FILTER GAME_SOURCES EXCLUDE REGEX "${THIRDPARTY_DIR_GAME}/.*")

# Combine sources
set(SOURCES ${MAIN_SCRIPT})  # Initialize SOURCES with the main script
list(APPEND SOURCES ${DEV_SOURCES} ${GAME_SOURCES})  # Append additional sources

# Add the executable target
add_executable(game ${SOURCES})  # Create an executable target with SOURCES

# Include directories for the target
target_include_directories(game PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/include 
    ${CMAKE_SOURCE_DIR}/src/utils/include 
    ${CMAKE_SOURCE_DIR}/game/include
)

# Custom target to force recompilation of the main script
add_custom_target(
    force_recompile_main ALL
    COMMAND ${CMAKE_COMMAND} -E touch ${MAIN_SCRIPT}
    COMMENT "Force recompilation."
)

# Make the 'game' target depend on this custom target
add_dependencies(game force_recompile_main)

# Panda3D setup
if(EXISTS "${CMAKE_SOURCE_DIR}/config.cmake")
    include("${CMAKE_SOURCE_DIR}/config.cmake")
else()
    message(FATAL_ERROR "Config file not found: ${CMAKE_SOURCE_DIR}/config.cmake. Please create it.")
endif()

# Find Panda3D libraries
find_library(PANDA_FRAMEWORK libp3framework PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDA_LIB libpanda PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDAEXPRESS_LIB libpandaexpress PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOL_LIB libp3dtool PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOLCONFIG_LIB libp3dtoolconfig PATHS ${PANDA3D_LIBRARY_DIR})

# Check if libraries were found
if(NOT PANDA_FRAMEWORK OR NOT PANDA_LIB OR NOT PANDAEXPRESS_LIB OR NOT DTOOL_LIB OR NOT DTOOLCONFIG_LIB)
    message(FATAL_ERROR "One or more Panda3D libraries were not found. Please check your installation.")
endif()

# Include directories for Panda3D
target_include_directories(game PRIVATE ${PANDA3D_INCLUDE_DIR})

# Link Panda3D libraries to the target
target_link_libraries(game PRIVATE ${PANDA_FRAMEWORK} ${PANDA_LIB} ${PANDAEXPRESS_LIB} ${DTOOL_LIB} ${DTOOLCONFIG_LIB})

# ImGui setup
set(IMGUI_DIR ${THIRDPARTY_DIR_SRC}/imgui)

# Check if ImGui exists and link if found
if(EXISTS ${IMGUI_DIR})
    message(STATUS "ImGui found, linking with the project.")
    target_include_directories(game PRIVATE ${IMGUI_DIR})

    # Link ImGui source files manually
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

# Set C++ standard (uncomment if required)
# set_target_properties(game PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# Post-build commands for running the game (optional)
if(FALSE)  # set to true to enable automatic running after build
    if(WIN32)
        add_custom_command(TARGET game
            POST_BUILD
            COMMAND $<IF:$<CONFIG:Release>,$<TARGET_FILE:game>,>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running game.exe in Release mode on Windows..."
        )
    elseif(UNIX)
        add_custom_command(TARGET game
            POST_BUILD
            COMMAND $<TARGET_FILE:game>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running game after build on Unix-like platforms..."
        )
    endif()
endif()

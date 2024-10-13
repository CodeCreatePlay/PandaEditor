cmake_minimum_required(VERSION 3.14)
project(PandaEditor LANGUAGES CXX)

# Ensure Visual Studio compiler is used on Windows
if(WIN32)
    if(NOT MSVC)
        message(FATAL_ERROR "This project requires Microsoft Visual Studio compiler on Windows.")
    endif()
else()
    # On non-Windows platforms, allow CMake to auto-detect compilers
    message(STATUS "Auto-detecting compiler for non-Windows platform.")
endif()

# Paths
set(THIRDPARTY_DIR_SRC ${CMAKE_SOURCE_DIR}/src/thirdparty)
set(THIRDPARTY_DIR_GAME ${CMAKE_SOURCE_DIR}/game/thirdparty)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(GAME_DIR ${CMAKE_SOURCE_DIR}/game)

# Gather source files
file(GLOB_RECURSE DEV_SOURCES ${SOURCE_DIR}/*.cpp)
list(FILTER DEV_SOURCES EXCLUDE REGEX "${THIRDPARTY_DIR_SRC}/.*")
file(GLOB_RECURSE GAME_SOURCES ${GAME_DIR}/*.cpp)
list(FILTER GAME_SOURCES EXCLUDE REGEX "${THIRDPARTY_DIR_GAME}/.*")
set(SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
list(APPEND SOURCES ${DEV_SOURCES} ${GAME_SOURCES})



# Add executable
add_executable(game ${SOURCES})
target_include_directories(game PRIVATE ${CMAKE_SOURCE_DIR}/src/include ${CMAKE_SOURCE_DIR}/game/include)

# Custom target to touch main.cpp
add_custom_target(
    force_recompile_main ALL
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/main.cpp
    COMMENT "Touching main.cpp to force recompilation"
)

# Make the 'game' target depend on this 'force_recompile_main' target
add_dependencies(game force_recompile_main)

# Panda3D setup
if(EXISTS "${CMAKE_SOURCE_DIR}/config.cmake")
    include("${CMAKE_SOURCE_DIR}/config.cmake")
else()
    message(FATAL_ERROR "Config file not found: ${CMAKE_SOURCE_DIR}/config.cmake. Please create it.")
endif()

find_library(PANDA_FRAMEWORK libp3framework PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDA_LIB libpanda PATHS ${PANDA3D_LIBRARY_DIR})
find_library(PANDAEXPRESS_LIB libpandaexpress PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOL_LIB libp3dtool PATHS ${PANDA3D_LIBRARY_DIR})
find_library(DTOOLCONFIG_LIB libp3dtoolconfig PATHS ${PANDA3D_LIBRARY_DIR})

target_include_directories(game PRIVATE ${PANDA3D_INCLUDE_DIR})
target_link_libraries(game PRIVATE ${PANDA_FRAMEWORK} ${PANDA_LIB} ${PANDAEXPRESS_LIB} ${DTOOL_LIB} ${DTOOLCONFIG_LIB})

# ImGUI setup
# Define the path to the ImGui directory (since it's downloaded via batch script)
set(IMGUI_DIR ${THIRDPARTY_DIR_SRC}/imgui)

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

# C++ standard
# set_target_properties(game PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# Post-build command to automatically run the game after the build
if(WIN32)
    # For Windows (Visual Studio or any other tool)
    add_custom_command(TARGET game
        POST_BUILD
        COMMAND $<IF:$<CONFIG:Release>,$<TARGET_FILE:game>,>
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}  # Set working directory
        COMMENT "Running game.exe in Release mode on Windows..."
    )
elseif(UNIX)
    # For Linux and macOS
    add_custom_command(TARGET game
        POST_BUILD
        COMMAND $<TARGET_FILE:game>
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}  # Set working directory
        COMMENT "Running game after build on Unix-like platforms..."
    )
endif()

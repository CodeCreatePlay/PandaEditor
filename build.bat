@echo off
REM Set up environment variables (if necessary)
::set PANDA3D_ROOT=C:\Panda3D-1.10.13-x64
::set PATH=%PANDA3D_ROOT%\bin;%PATH%

REM Define thirdparty directory to store dependencies
set THIRDPARTY_DIR=%CD%\src\thirdparty
set IMGUI_VERSION=v1.91.2

REM Ensure thirdparty directory exists
if not exist "%THIRDPARTY_DIR%" (
    echo Creating thirdparty directory...
    mkdir "%THIRDPARTY_DIR%"
)

REM 1. Download ImGui from GitHub if not already downloaded
if not exist "%THIRDPARTY_DIR%\imgui" (
    echo Downloading ImGui from GitHub...
    git clone https://github.com/ocornut/imgui.git "%THIRDPARTY_DIR%\imgui"
    
    REM Check out the specified version of ImGui
    cd "%THIRDPARTY_DIR%\imgui"
    git checkout %IMGUI_VERSION%
    cd ../../..) else (
    echo ImGui already downloaded.)

REM 2. Run CMake to configure and build the project
:: -Bbuild: The -B option specifies the build directory, where the build files (like Makefiles, Visual Studio project files, etc.) will be generated. In this case, build is the directory name, so all the generated files will be placed in a directory called build. If the directory doesn't exist, CMake will create it.
:: -S.: The -S option specifies the source directory. The . means the current directory is used as the source for CMake to look for CMakeLists.txt, which contains the build configuration.
echo Configuring the project with CMake...
cmake -Bbuild -S. -A x64

echo Building the project...
cmake --build build --config Release

REM 3. Optionally, run the executable after the build
REM This step is done at the end of cmake as a add_custom_command.
::if exist "build\Release\game.exe" &&  (
::    echo Running the built game executable...
::    build\Release\game.exe
::) else (
::    echo Build failed or executable not found!
::)

REM End of the script
pause
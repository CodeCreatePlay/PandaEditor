# PandaEditor
**An effort to make Panda3D more robust and user-friendly for artists and game developers by adding scene and level editor features, along with a new rendering pipeline. This project when completed will enable users to design, visualize, and organize projects directly within the editor, providing a cohesive environment to streamline the development process.**

### Prerequisites
- Git
- CMake (Version 3.14 or higher)
- Panda3D SDK with development libraries
- C++ Compiler
   - Windows: Microsoft Visual Studio (with MSVC)
   - Linux or macOS: GCC or Clang
- curl and unzip for downloading and extracting assets (available by default on most systems)
- Other dependencies are downloaded at runtime.

### Cloning the Repository

- To clone this repository,

	```
	git clone <https://github.com/CodeCreatePlay/PandaEditor>
	```

- Alternatively, you can manually download it as a ZIP file.

### Configuring CMake
Update the config.cmake file in the root directory to set up the paths for Panda3D, as shown below.

```
# Example config.cmake file

# Panda3D library and include paths
set(PANDA3D_LIBRARY_DIR "path_to_your_panda3d_lib_directory")
set(PANDA3D_INCLUDE_DIR "path_to_your_panda3d_include_directory")
```

### Starting the build
Run the build.sh script and wait for the build to finish. The executable should launch automatically at the end. To start it manually later, you can find it in the build/Release (or equivalent) directory.

### Running demo projects
Demos are located in the demos folder. To start a demo project, specify the name of the demo project as a command-line argument to the build script.

```
# bash

# Example for running a demo project
source build.sh roaming-ralph
```

### Project
The game directory is the projects directory. CMake will automatically detect and link all source files from this directory.

### Common Issues
- Panda3D Library Not Found: Verify config.cmake contains the correct paths.
- Unsupported Compiler: Ensure you're using a supported compiler (MSVC on Windows; GCC or Clang elsewhere).
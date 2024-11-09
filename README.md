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

### Cloning the repository

- To clone this repository,

	```
	git clone <https://github.com/CodeCreatePlay/PandaEditor>
	```

- Alternatively, you can manually download it as a ZIP file.

### Configuring CMake
Update the `config.cmake` file in the root directory to set up the paths for Panda3D, as shown below.

```
# Example config.cmake file
# Panda3D library and include paths
set(PANDA3D_LIBRARY_DIR "path_to_your_panda3d_lib_directory")
set(PANDA3D_INCLUDE_DIR "path_to_your_panda3d_include_directory")
```

### Starting the build
Use the included `build.sh` script to start the build. The executable should launch automatically at the end. To start it manually later, you can find it in `builds/project-name/Release` (or equivalent) directory. 
 
**Creating a new project:** The `game` directory is the default directory for projects, the build supports a project-based approach, allowing you to specify a custom project and an appropriate build directory is created for each project.

- **Default Behavior:** If no specific project is specified, build system will create a `default-project` build folder and all source files from `game` directory are linked.

- **Project-Based Builds:** To start a new project you specify the optional command line argument `USER_PROJECT` which is the name of the project you want to start, for example.

```
# bash
# replace 'your-project-name' with the name of your project
source build.sh your-project-name
```

The system will search for specified project in two directories `game` and `demos`, then source files from only the specified project directory are included.

Here is an example of how the project strcuture should look like.

```
src/
├── game/                     # Main directory for user-defined projects
│   ├── UserProject           # Example user project
│   ├── AnotherProject        # Another example user project
│   └── YetAnotherProject     # Additional example user project
│
├── demos/                    # Directory for demo projects
│   └── roaming-ralph         # Demo project: Roaming Ralph
│
├── builds/                   # Directory for build output
│   ├── UserProject           # Build output for UserProject
│   ├── AnotherProject        # Build output for AnotherProject
│   ├── YetAnotherProject     # Build output for YetAnotherProject
│   └── roaming-ralph         # Build output for Roaming Ralph demo

```

### Running demo projects
Demos are located in the `demos` folder. To start a demo project, specify the name of the demo project as a command-line argument to the build script.

```
# bash
# Example for running a demo project
source build.sh roaming-ralph
```

### Common Issues
- Panda3D Library Not Found: Verify `config.cmake` contains the correct paths.
- Unsupported Compiler: Ensure you're using a supported compiler (MSVC on Windows; GCC or Clang elsewhere).
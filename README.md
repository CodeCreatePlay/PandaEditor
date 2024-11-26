# PandaEditor
**An effort to make Panda3D more robust and user-friendly for artists and game developers by adding scene and level editor features, along with a new rendering pipeline. This project when completed will enable users to design, visualize, and organize projects directly within the editor, providing a cohesive environment to streamline the development process.**

## TableOfContents
1. [Building and Configuring PandaEditor Projects]()
2. [HelloWorld Tutorial]()

<h2 align="center">Building and Configuring PandaEditor Projects</h2>

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
Use the included `build.sh` script to start the build. The executable should launch automatically at the end. To start it manually later, you can find it in `builds/project-name/Release` (or equivalent) directory. The `build.sh` script will prompt you to enter the project name. If the specified project is not found, you'll have the option to create it; otherwise, the script would exit.
 
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

### Common Issues
- Panda3D Library Not Found: Verify `config.cmake` contains the correct paths.
- Unsupported Compiler: Ensure you're using a supported compiler (MSVC on Windows; GCC or Clang elsewhere).

<h2 align="center">HelloWorld tutorial</h2>

PandaEditor uses the `Demon` class to set up and initialize the underlying Panda3D game engine and editor environment. To start PandaEditor with minimal setup, you only need to create an instance of the `Demon` class and call its start method.

```
#include "demon.h"

int main(int argc, char* argv[]) {
    // Create an instance of the Demon class
    Demon demon;

    // your code goes here...

    // Initialize and start the PandaEditor
    demon.start();

    // Exit the program
    return 0;  
}

```

Or you can subclass from `Demon` class itself, also this is the recommended approach.

```
#include "Demon.h"

class MyApp : public Demon {
public:
    MyApp() { 
        // your code goes here...
    }
};


int main(int argc, char* argv[]) {
    MyApp app;
    app.start();
    return 0;
}
```

**Create a New Project Directory** Create a new directory in `game` folder and rename it appropriately for example 'HelloWorldProject'; each folder in `game` represents a unique folder, put all code related to a particular project in its corresponding project folder in `game` directory.
	
**Create a Source File** Inside your new project folder, create a source file. Then, copy and paste one of the example code snippets provided above into this file.

**Build the Project** Back in PandaEditor main directory, start the `build.sh` script as explained in `Starting the build` section above, to start the build.

**Starting Demo projects** `Demos` are projects just like user defined projects in `game` folder, to start a demo, simply enter the name of the demo as the project name, when build starts, the build would proceed as explained above.
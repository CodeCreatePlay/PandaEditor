#include "include/Project.h"


Project::Project() {}

void Project::set_project(const std::string& path) {
    // Ensure the path exists and is a directory
    assert(PathUtils::file_exists(path) && "Path does not exist.");
    assert(PathUtils::is_dir(path.c_str()) && "Path is not a directory.");

    // Set the project name and path
    this->name = "PandaEditorProject";
    this->path = path;

    // Clear Panda3D's current model paths and set a new one according to the new project path
    get_model_path().prepend_directory(Filename::from_os_specific(path));

    std::cout << "-- Project created successfully" << std::endl;
}

void Project::reload() {
    // Reload project data or resources
}

const std::string& Project::get_path() const { return path; }
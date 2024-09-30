#ifndef Project_H
#define Project_H

#include <string>
#include "config_putil.h"
#include "..//utils/include/pathUtils.h"


class Project {
public:
    // Constructor
    Project();

    // Set the project path
    void set_project(const std::string& path);

    // Reload project
    void reload();

    // Getter for the project path
    const std::string& get_path() const;

private:
    std::string name;
    std::string path;
};

#endif
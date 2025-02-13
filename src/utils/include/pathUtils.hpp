#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <string>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>    // For Windows _getcwd
#define getcwd _getcwd // Map POSIX name to Windows name
#include <limits.h>
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>    // For Linux/Unix getcwd
#include <limits.h>
#else
#error "Unsupported operating system."
#endif


class PathUtils {
public:
    static inline std::string get_current_working_dir();
	static inline std::string join_paths(const std::string& base, const std::string& folder);
    static inline bool file_exists(const std::string& path);
    static inline bool is_dir(const char* path);
    static inline bool is_file(const char* path);
};

/// <summary>
/// Checks if file exists, in C++ 17 and above use std::filesystem.
/// </summary>
/// <param name="name"></param>
/// <returns></returns>
inline bool PathUtils::file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

inline bool PathUtils::is_dir(const char* path) {
    struct stat s;
    if (stat(path, &s) == 0 && s.st_mode & S_IFDIR)
        return true;  // it's a directory

    return false;
}

inline bool PathUtils::is_file(const char* path) {
    struct stat s;
    if (stat(path, &s) == 0 && s.st_mode & S_IFREG)
        return true;  // its a file

    return false;
}

inline std::string PathUtils::get_current_working_dir()
{
    char cwd[_MAX_PATH];

    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    }
    else {
        return std::string();
    }
}

inline std::string PathUtils::join_paths(const std::string& base, const std::string& folder) {
    // Check if the base path ends with a separator, and add one if necessary
    char separator = '/';  // Use '/' for POSIX systems, or '\\' for Windows
#ifdef _WIN32
    separator = '\\';
#endif

    if (!base.empty() && base.back() != separator) {
        return base + separator + folder;
    } else {
        return base + folder;
    }
}

#endif

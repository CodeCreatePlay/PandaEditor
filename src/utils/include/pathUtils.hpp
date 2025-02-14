#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <string>
#include <iostream>
#include <sys/stat.h>  // File/directory checks
#include <algorithm>   // For std::replace
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>    
    #include <limits.h>    // For _MAX_PATH
    #define getcwd _getcwd 
    #define PATH_SEPARATOR '\\'
    #ifndef PATH_MAX
        #define PATH_MAX _MAX_PATH   // Define PATH_MAX for Windows
    #endif
#else
    #include <unistd.h>    
    #include <limits.h>    // PATH_MAX is defined here for Linux/macOS
    #define PATH_SEPARATOR '/'
#endif

class PathUtils {
public:
    static inline std::string get_current_working_dir();
    static inline std::string join_paths(const std::string& base, const std::string& folder);
    static inline bool file_exists(const std::string& path);
    static inline bool is_dir(const std::string& path);
    static inline bool is_file(const std::string& path);
    static inline std::string to_os_specific(const std::string& path);
	static inline std::string to_engine_specific(const std::string& path);
};

/// <summary>
/// Checks if file exists. In C++17 and above, prefer std::filesystem.
/// </summary>
inline bool PathUtils::file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

/// <summary>
/// Checks if the given path is a directory.
/// </summary>
inline bool PathUtils::is_dir(const std::string& path) {
    struct stat s;
    return (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFDIR));
}

/// <summary>
/// Checks if the given path is a file.
/// </summary>
inline bool PathUtils::is_file(const std::string& path) {
    struct stat s;
    return (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFREG));
}

/// <summary>
/// Gets the current working directory.
/// </summary>
inline std::string PathUtils::get_current_working_dir() {
    std::vector<char> buffer(PATH_MAX, '\0');  
    if (getcwd(buffer.data(), buffer.size()) != nullptr) {
        return std::string(buffer.data());
    }
    return std::string();
}

/// <summary>
/// Joins two path segments using the correct OS-specific separator.
/// </summary>
inline std::string PathUtils::join_paths(const std::string& base, const std::string& folder) {
    if (base.empty()) return folder;
    if (folder.empty()) return base;

    if (base.back() != PATH_SEPARATOR) {
        return base + PATH_SEPARATOR + folder;
    } else {
        return base + folder;
    }
}

/// <summary>
/// Converts a given path to match the OS-specific separator.
/// </summary>
inline std::string PathUtils::to_os_specific(const std::string& path) {
    std::string converted_path = path;

#if defined(_WIN32) || defined(_WIN64)
    std::replace(converted_path.begin(), converted_path.end(), '/', '\\');
#else
    std::replace(converted_path.begin(), converted_path.end(), '\\', '/');
#endif

    return converted_path;
}

/// <summary>
/// Converts a given path to engine specific.
/// </summary>
inline std::string PathUtils::to_engine_specific(const std::string& path) {
    std::string converted_path = path;
    std::replace(converted_path.begin(), converted_path.end(), '\\', '/');
    return converted_path;
}

#endif // PATHUTILS_H

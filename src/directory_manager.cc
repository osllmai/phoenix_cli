//
// Created by Amir Kouhkan on 7/30/24.
//

#include "directory_manager.h"

#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else

#include <unistd.h>

#endif

#include <filesystem>


namespace fs = std::filesystem;

std::string DirectoryManager::get_home_directory() {
#if defined(_WIN32) || defined(_WIN64)
    char home_path[MAX_PATH];
    if (GetEnvironmentVariable("USERPROFILE", home_path, MAX_PATH)) {
        return std::string(home_path);
    }
    return "";
#else
    const char *home_directory = getenv("HOME");
    if (home_directory) {
        return std::string(home_directory);
    }
    return "";
#endif
}

bool DirectoryManager::create_home_directory(const std::string &path) {
#if defined(_WIN32) || defined(_WIN64)
    return CreateDirectory(path.c_str(), NULL) != 0;
#else
    return mkdir(path.c_str(), 0755) == 0;
#endif
}

void DirectoryManager::handle_application_directory() {
    std::string home_directory = get_home_directory();
    if (home_directory.empty()) {
        std::cerr << "Unable to find home directory" << std::endl;
        return;
    }

    std::string app_dir = home_directory + "/.phoenix";

    if (create_home_directory(app_dir)) {
        std::cout << "Home directory for application crated" << std::endl;
    } else {
        std::cerr << "Already home directory created" << std::endl;
        return;
    }
}

std::string DirectoryManager::get_app_home_path() {
    std::string home_path = get_home_directory();
    return home_path + "/.phoenix";
}


std::string DirectoryManager::find_llm_in_app_home(const std::string &filename) {
    std::string ext = "gguf";
    if (ext.front() != '.') {
        ext = '.' + ext;
    }

    std::string full_filename = filename + ext;

    for (const auto &entry: fs::directory_iterator(get_app_home_path())) {
        if (entry.is_regular_file() && entry.path().filename() == full_filename) {
            return entry.path().string();
        }
    }

    std::cerr << filename << " is not located or downloaded" << std::endl;
    return "";
}

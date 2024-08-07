//
// Created by Amir Kouhkan on 7/30/24.
//

#include "directory_manager.h"
#include "models_list.h"

#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


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

    create_home_directory(app_dir);
    /*if () {
       std::cout << "Home directory for application crated" << std::endl;
   } else {
       std::cerr << "Already home directory created" << std::endl;
       return;
   }*/
}

std::string DirectoryManager::get_app_home_path() {
    std::string home_path = get_home_directory();
    return home_path + "/.phoenix";
}


std::string DirectoryManager::find_llm_in_app_home(const std::string &filename) {
    json models_list = list_of_models_available();
    if (!models_list.contains(filename)) {
        std::cerr << "Model not found" << std::endl;
        return "";
    }

    std::string model_company = models_list[filename]["companyName"].get<std::string>();
    std::string path = get_app_home_path() + "/models/" + model_company + "/";

    std::string ext = "gguf";
    if (ext.front() != '.') {
        ext = '.' + ext;
    }

    std::string full_filename = filename + ext;

    for (const auto &entry: fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().filename() == full_filename) {
            return entry.path().string();
        }
    }

    std::cerr << filename << " is not located or downloaded" << std::endl;
    return "";
}


std::vector<std::string> DirectoryManager::local_models() {
    std::vector<std::string> models{};
    std::string models_path = get_app_home_path() + "/models/";

    for (const auto &company_entry : fs::directory_iterator(models_path)) {
        if (company_entry.is_directory()) {
            for (const auto &model_entry : fs::directory_iterator(company_entry.path())) {
                if (model_entry.is_regular_file()) {
                    models.push_back(model_entry.path().filename().string());
                }
            }
        }
    }

    return models;
}

bool DirectoryManager::create_chats_directory() {
    std::string directory_name = "chats";
    try {
        if (fs::create_directory(directory_name)) {
            return true;
        } else {
            return false;
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

bool DirectoryManager::create_custom_directory(const std::string &path, const std::string &directory_name) {
    std::string directory = path + "/" + directory_name;
    try {
        if (fs::create_directory(directory)) {
            return true;
        } else {
            return false;
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}
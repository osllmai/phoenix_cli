#include "directory_manager.h"
#include "models_list.h"

#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

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
    return home_directory ? std::string(home_directory) : "";
#endif
}

bool DirectoryManager::create_directory(const std::string &path) {
    try {
        return fs::create_directory(path);
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

void DirectoryManager::initialize_app_directory() {
    std::string app_dir = get_app_home_path();
    if (!create_directory(app_dir)) {
        std::cerr << "Failed to create application directory at " << app_dir << std::endl;
    }
}

std::string DirectoryManager::get_app_home_path() {
    return get_home_directory() + "/.phoenix";
}

std::string DirectoryManager::find_model_in_app(const std::string &filename) {
    json models_list = PhoenixModelList::list_of_models_available();
    if (!models_list.contains(filename)) {
        std::cerr << "Model not found" << std::endl;
        return "";
    }

    std::string path = get_model_path(models_list[filename]);
    std::string full_filename = filename + ".gguf";

    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().filename() == full_filename) {
            return entry.path().string();
        }
    }

    std::cerr << filename << " is not located or downloaded" << std::endl;
    return "";
}

std::string DirectoryManager::get_model_path(const json &model_info) {
    return get_app_home_path() + "/models/" + model_info["companyName"].get<std::string>() + "/";
}

std::vector<std::string> DirectoryManager::list_local_models() {
    std::vector<std::string> models;
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
    return create_directory(get_app_home_path() + "/chats");
}

bool DirectoryManager::create_custom_directory(const std::string &base_path, const std::string &directory_name) {
    return create_directory(base_path + "/" + directory_name);
}

bool DirectoryManager::delete_model(const std::string &model_name) {
    try {
        json model_info = PhoenixModelList::model_data(model_name);
        std::string path = get_model_path(model_info);

        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().filename() == model_name + ".gguf") {
                fs::remove(entry.path());
                return true;
            }
        }
        std::cerr << model_name << " is not downloaded" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error deleting model: " << e.what() << std::endl;
    }

    return false;
}

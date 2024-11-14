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
    } else {
        std::cerr << "Error: Unable to retrieve USERPROFILE environment variable on Windows." << std::endl;
        return "";
    }
#else
    const char *home_directory = getenv("HOME");
    if (!home_directory) {
        std::cerr << "Error: HOME environment variable not set." << std::endl;
        return "";
    }
    return std::string(home_directory);
#endif
}

bool DirectoryManager::create_directory(const std::string &path) {
    try {
        if (fs::exists(path)) {
            if (fs::is_directory(path)) {
                return true; // Directory already exists
            } else {
                std::cerr << "Error: Path exists but is not a directory: " << path << std::endl;
                return false;
            }
        }
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
    std::string home_dir = get_home_directory();
    if (home_dir.empty()) {
        std::cerr << "Error: Unable to get home directory." << std::endl;
        return "";
    }
    return home_dir + "/.phoenix";
}

std::string DirectoryManager::find_model_in_app(const std::string &filename) {
    try {
        json models_list = PhoenixModelList::list_of_models_available();
        if (!models_list.contains(filename)) {
            std::cerr << "Error: Model not found in the list." << std::endl;
            return "";
        }

        std::string path = get_model_path(models_list[filename]);
        if (!fs::exists(path)) {
            std::cerr << "Error: Model path does not exist: " << path << std::endl;
            return "";
        }

        std::string full_filename = filename + ".gguf";
        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().filename() == full_filename) {
                return entry.path().string();
            }
        }

        std::cerr << "Error: Model file " << full_filename << " is not located or downloaded." << std::endl;
        return "";
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error accessing model in app: " << e.what() << std::endl;
        return "";
    } catch (const json::exception &e) {
        std::cerr << "Error with model JSON data: " << e.what() << std::endl;
        return "";
    }
}

std::string DirectoryManager::get_model_path(const json &model_info) {
    try {
        std::string company_name = model_info.at("companyName").get<std::string>();
        return get_app_home_path() + "/models/" + company_name + "/";
    } catch (const json::exception &e) {
        std::cerr << "Error accessing companyName in model info: " << e.what() << std::endl;
        return "";
    }
}

std::vector<std::string> DirectoryManager::list_local_models() {
    std::vector<std::string> models;
    std::string models_path = get_app_home_path() + "/models/";

//    TODO; Filestream search

    try {
        if (!fs::exists(models_path)) {
            std::cerr << "Error: Models path does not exist: " << models_path << std::endl;
            return models;
        }

        for (const auto &company_entry : fs::directory_iterator(models_path)) {
            if (company_entry.is_directory()) {
                for (const auto &model_entry : fs::directory_iterator(company_entry.path())) {
                    if (model_entry.is_regular_file()) {
                        models.push_back(model_entry.path().filename().string());
                    }
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error listing local models: " << e.what() << std::endl;
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
//    TODO; remove file by filesystem remove method

    try {
        json model_info = PhoenixModelList::model_data(model_name);
        std::string path = get_model_path(model_info);

        if (!fs::exists(path)) {
            std::cerr << "Error: Model path does not exist: " << path << std::endl;
            return false;
        }

        std::string model_file = model_name + ".gguf";
        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().filename() == model_file) {
                fs::remove(entry.path());
                return true;
            }
        }
        std::cerr << "Error: Model file " << model_file << " is not downloaded." << std::endl;
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error deleting model file: " << e.what() << std::endl;
    } catch (const json::exception &e) {
        std::cerr << "Error with model JSON data: " << e.what() << std::endl;
    }

    return false;
}

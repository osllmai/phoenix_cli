#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class DirectoryManager {
public:
    using json = nlohmann::json;

    static std::string get_home_directory();

    static bool create_directory(const std::string &path);

    static void initialize_app_directory();

    static std::string get_app_home_path();

    static std::string find_model_in_app(const std::string &filename);

    static std::vector<std::string> list_local_models();

    static bool create_chats_directory();

    static bool create_custom_directory(const std::string &base_path, const std::string &directory_name);

    static bool delete_model(const std::string &model_name);

private:
    static std::string get_model_path(const json &model_info);
};

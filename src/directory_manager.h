//
// Created by Amir Kouhkan on 7/30/24.
//

#ifndef PHOENIX_DIRECTORY_MANAGER_H
#define PHOENIX_DIRECTORY_MANAGER_H

#include <string>
#include <vector>


class DirectoryManager {
public:
    static std::string get_home_directory();

    static bool create_home_directory(const std::string &path);

    static void handle_application_directory();

    static std::string get_app_home_path();

    static std::string find_llm_in_app_home(const std::string &filename);

    static std::vector<std::string> local_models();

    static bool create_chats_directory();

    static bool create_custom_directory(const std::string &path, const std::string &directory_name);

    static bool delete_model(const std::string &model_name);
};


#endif //PHOENIX_DIRECTORY_MANAGER_H

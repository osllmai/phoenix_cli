#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserFolder {
    int id;
    std::string user_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
    std::string name;
    std::string description;
    std::string type;

    UserFolder(int id,
               std::string user_id,
               int workspace_id,
               std::string created_at,
               std::string updated_at,
               std::string name,
               std::string description,
               std::string type)
            : id(std::move(id)),
              user_id(std::move(user_id)),
              workspace_id(std::move(workspace_id)),
              created_at(std::move(created_at)),
              updated_at(std::move(updated_at)),
              name(std::move(name)),
              description(std::move(description)),
              type(std::move(type)) {}


    UserFolder() = default;

};

namespace models {
    class Folder {
    public:
        static void to_json(json &j, const UserFolder &user_folder);

        static void create_folder(const UserFolder &user_folder);

        static bool update_folder(const int &folder_id, const UserFolder &user_folder);

        static bool delete_folder(const int &folder_id);

        static UserFolder get_folder_by_id(const int &folder_id);

        static std::vector<UserFolder> user_folders_by_user(const std::string &user_id);

        static std::vector<UserFolder> user_folders_by_workspace(const int &workspace_id);
    };
}
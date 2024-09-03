#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserFile {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    std::string description;
    std::string file_path;
    std::string name;
    int size;
    int tokens;
    std::string type;

    UserFile() = default;

    UserFile(const int &id, const std::string &user_id, const int &folder_id,
         const std::string &created_at, const std::string &updated_at, const std::string &sharing,
         const std::string &description, const std::string &file_path, const std::string &name, int size, int tokens,
         const std::string &type)
            : id(id), user_id(user_id), folder_id(folder_id), created_at(created_at), updated_at(updated_at),
              sharing(sharing), description(description), file_path(file_path), name(name), size(size), tokens(tokens),
              type(type) {}
};

namespace models {
    class File {
    public:
        static void create_file(const UserFile &user_file);

        static UserFile get_file_by_id(const int &file_id);

        static bool update_file(const int &file_id, const UserFile &user_file);

        static bool delete_file(const int &file_id);

        static std::vector<UserFile> files_by_user(const std::string &user_id);

        static std::vector<UserFile> files_by_folder(const int &folder_id);

        static void to_json(json &j, const UserFile &user_file);
    };
}

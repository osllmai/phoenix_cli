#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserCollectionFile {
    std::string user_id;
    int collection_id;
    int file_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class CollectionFile {
    public:
        static UserCollectionFile get_collection_file_by_id(const int &collection_id, const int &file_id);
        static void create_collection_file(const UserCollectionFile &collection_file);
        static bool update_collection_file(const int &collection_id, const int &file_id, const UserCollectionFile &collection_file);
        static bool delete_collection_file(const int &collection_id, const int &file_id);
        static std::vector<UserCollectionFile> collection_files(const std::string &user_id);
        static void to_json(json &j, const UserCollectionFile &collection_file);
    };
}

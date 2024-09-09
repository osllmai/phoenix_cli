#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserCollection {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    std::string description;
    std::string name;
};

namespace models {
    class Collection {
    public:
        static UserCollection get_collection_by_id(const int &id);
        static void create_collection(const UserCollection &collection);
        static bool update_collection(const int &id, const UserCollection &collection);
        static bool delete_collection(const int &id);
        static std::vector<UserCollection> collections(const std::string &user_id);
        static void to_json(json &j, const UserCollection &collection);
    };
}

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserAssistantCollection {
    std::string user_id;
    int assistant_id;
    int collection_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class AssistantCollection {
    public:
        static UserAssistantCollection get_assistant_collection_by_id(const int &assistant_id, const int &collection_id);
        static void create_assistant_collection(const UserAssistantCollection &assistant_collection);
        static bool update_assistant_collection(const int &assistant_id, const int &collection_id, const UserAssistantCollection &assistant_collection);
        static bool delete_assistant_collection(const int &assistant_id, const int &collection_id);
        static std::vector<UserAssistantCollection> assistant_collections(const std::string &user_id);
        static void to_json(json &j, const UserAssistantCollection &assistant_collection);
    };
}

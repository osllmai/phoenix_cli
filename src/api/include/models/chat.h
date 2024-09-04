#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserChat {
    int id;
    std::string user_id;
    int workspace_id;
    int assistant_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    int context_length;
    std::string description;
    std::string embeddings_provider;
    bool include_profile_context;
    bool include_workspace_instructions;
    std::string model;
    std::string name;
    std::string prompt;
    double temperature;
};

namespace models {
    class Chat {
    public:
        static UserChat get_chat_by_id(const int &id);

        static void create_chat(const UserChat &user_chat);

        static bool update_chat(const int &id, const UserChat &user_chat);

        static bool delete_chat(const int &id);

        static std::vector<UserChat> chats(const std::string &user_id);

        static void to_json(json &j, const UserChat &chat);
    };
}

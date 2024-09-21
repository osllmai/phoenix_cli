#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserAssistant {
    int id;
    std::string user_id;
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
    std::string image_path;
    std::string prompt;
    double temperature;
};

namespace models {
    class Assistant {
    public:
        static UserAssistant get_assistant_by_id(const int &id);

        static void create_assistant(const UserAssistant &user_assistant);

        static bool update_assistant(const int &id, const UserAssistant &user_assistant);

        static bool delete_assistant(const int &id);

        static std::vector<UserAssistant> assistants(const std::string &user_id);

        static std::vector<UserAssistant> get_assistants_by_workspace_id(const int &workspace_id);

        static void to_json(json &j, const UserAssistant &assistant);
    };
}

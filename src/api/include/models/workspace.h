#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserWorkspace {
    std::string id;
    std::string user_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    int default_context_length;
    std::string default_model;
    std::string default_prompt;
    double default_temperature;
    std::string description;
    std::string embeddings_provider;
    bool include_profile_context;
    bool include_workspace_instructions;
    std::string instructions;
    bool is_home;
    std::string name;
};


namespace models {
    class Workspace {
    public:
        static UserWorkspace get_workspace_by_id(const int &id);

        static void create_workspace(const UserWorkspace &user_workspace);

        static bool update_workspace(const int &id, const UserWorkspace &user_workspace);

        static bool delete_workspace(const int &id);

        static std::vector<UserWorkspace> workspaces(const std::string &user_id);

        static void disable_home_workspaces();

        static void to_json(json &j, const UserWorkspace &workspace);


    };
}


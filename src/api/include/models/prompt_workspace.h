#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserPromptWorkspace {
    std::string user_id;
    int prompt_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class PromptWorkspace {
    public:
        static UserPromptWorkspace get_prompt_workspace_by_id(const int &prompt_id, const int &workspace_id);
        static void create_prompt_workspace(const UserPromptWorkspace &prompt_workspace);
        static bool update_prompt_workspace(const int &prompt_id, const int &workspace_id, const UserPromptWorkspace &prompt_workspace);
        static bool delete_prompt_workspace(const int &prompt_id, const int &workspace_id);
        static std::vector<UserPromptWorkspace> prompt_workspaces(const std::string &user_id);
        static void to_json(json &j, const UserPromptWorkspace &prompt_workspace);
    };
}

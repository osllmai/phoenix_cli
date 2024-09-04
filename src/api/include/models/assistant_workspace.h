#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserAssistantWorkspace {
    std::string user_id;
    int assistant_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class AssistantWorkspace {
    public:
        static UserAssistantWorkspace get_assistant_workspace_by_id(const int &assistant_id, const int &workspace_id);

        static void create_assistant_workspace(const UserAssistantWorkspace &user_assistant_workspace);

        static bool update_assistant_workspace(const int &assistant_id, const int &workspace_id,
                                               const UserAssistantWorkspace &user_assistant_workspace);

        static bool delete_assistant_workspace(const int &assistant_id, const int &workspace_id);

        static std::vector<UserAssistantWorkspace> assistant_workspaces(const std::string &user_id);

        static void to_json(json &j, const UserAssistantWorkspace &assistant_workspace);

    };
}
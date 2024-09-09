#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserToolWorkspace {
    std::string user_id;
    int tool_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class ToolWorkspace {
    public:
        static UserToolWorkspace get_tool_workspace_by_id(const int &tool_id, const int &workspace_id);
        static void create_tool_workspace(const UserToolWorkspace &tool_workspace);
        static bool update_tool_workspace(const int &tool_id, const int &workspace_id, const UserToolWorkspace &tool_workspace);
        static bool delete_tool_workspace(const int &tool_id, const int &workspace_id);
        static std::vector<UserToolWorkspace> tool_workspaces(const std::string &user_id);
        static void to_json(json &j, const UserToolWorkspace &tool_workspace);
    };
}

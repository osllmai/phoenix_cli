#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserTool {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    std::string description;
    std::string name;
    json schema;
    std::string url;
};

namespace models {
    class Tool {
    public:
        static UserTool get_tool_by_id(const int &id);

        static void create_tool(const UserTool &tool);

        static bool update_tool(const int &id, const UserTool &tool);

        static bool delete_tool(const int &id);

        static std::vector<UserTool> tools(const std::string &user_id);

        static std::vector<UserTool> get_tools_by_workspace_id(const int &workspace_id);

        static void to_json(json &j, const UserTool &tool);
    };
}

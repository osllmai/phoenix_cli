#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserAssistantTool {
    std::string user_id;
    int assistant_id;
    int tool_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class AssistantTool {
    public:
        static UserAssistantTool get_assistant_tool_by_id(const int &assistant_id, const int &tool_id);
        static void create_assistant_tool(const UserAssistantTool &assistant_tool);
        static bool update_assistant_tool(const int &assistant_id, const int &tool_id, const UserAssistantTool &assistant_tool);
        static bool delete_assistant_tool(const int &assistant_id, const int &tool_id);
        static std::vector<UserAssistantTool> assistant_tools(const std::string &user_id);
        static std::vector<UserAssistantTool> get_assistant_tools_by_assistant_id(const int &assistant_id);
        static void to_json(json &j, const UserAssistantTool &assistant_tool);
    };
}

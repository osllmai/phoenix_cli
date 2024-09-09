#include "api/include/models/assistant_tool.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void AssistantTool::create_assistant_tool(const UserAssistantTool &assistant_tool) {
        db << R"(INSERT INTO assistant_tools (user_id, assistant_id, tool_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << assistant_tool.user_id
           << assistant_tool.assistant_id
           << assistant_tool.tool_id
           << assistant_tool.created_at
           << assistant_tool.updated_at;
    }

    UserAssistantTool AssistantTool::get_assistant_tool_by_id(const int &assistant_id, const int &tool_id) {
        UserAssistantTool assistant_tool;
        db << "SELECT * FROM assistant_tools WHERE assistant_id = ? AND tool_id = ?;"
           << assistant_id << tool_id >>
           [&](std::string user_id, int assistant_id, int tool_id, std::string created_at, std::string updated_at) {
               assistant_tool = {user_id, assistant_id, tool_id, created_at, updated_at};
           };
        return assistant_tool;
    }

    bool AssistantTool::update_assistant_tool(const int &assistant_id, const int &tool_id, const UserAssistantTool &assistant_tool) {
        try {
            db << "UPDATE assistant_tools SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE assistant_id = ? AND tool_id = ?"
               << assistant_tool.user_id
               << assistant_tool.created_at
               << assistant_tool.updated_at
               << assistant_id
               << tool_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating assistant_tool: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating assistant_tool: " << e.what() << std::endl;
            return false;
        }
    }

    bool AssistantTool::delete_assistant_tool(const int &assistant_id, const int &tool_id) {
        bool is_deleted = false;
        db << "DELETE FROM assistant_tools WHERE assistant_id = ? AND tool_id = ?;"
           << assistant_id << tool_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserAssistantTool> AssistantTool::assistant_tools(const std::string &user_id) {
        std::vector<UserAssistantTool> result;

        try {
            db << "SELECT user_id, assistant_id, tool_id, created_at, updated_at "
                  "FROM assistant_tools WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int assistant_id, int tool_id, std::string created_at, std::string updated_at) {
                   UserAssistantTool assistant_tool{
                           user_id, assistant_id, tool_id, created_at, updated_at};
                   result.push_back(assistant_tool);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistant_tools: " << e.what() << std::endl;
        }

        return result;
    }

    void AssistantTool::to_json(json &j, const UserAssistantTool &assistant_tool) {
        j = json{
                {"user_id",              assistant_tool.user_id},
                {"assistant_id",         assistant_tool.assistant_id},
                {"tool_id",              assistant_tool.tool_id},
                {"created_at",           assistant_tool.created_at},
                {"updated_at",           assistant_tool.updated_at}
        };
    };
}

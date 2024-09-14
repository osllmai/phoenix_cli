#include "api/include/models/tool.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Tool::create_tool(const UserTool &tool) {
        db << R"(INSERT INTO tools (user_id, folder_id, created_at, updated_at, sharing, description, name, schema, url)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << tool.user_id
           << tool.folder_id
           << tool.created_at
           << tool.updated_at
           << tool.sharing
           << tool.description
           << tool.name
           << tool.schema.dump()
           << tool.url;
    }

    UserTool Tool::get_tool_by_id(const int &id) {
        UserTool tool;
        db << "SELECT * FROM tools WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, std::string description, std::string name, std::string schema, std::string url) {
               tool = {id, user_id, folder_id, created_at, updated_at, sharing, description, name, json::parse(schema), url};
           };
        return tool;
    }

    bool Tool::update_tool(const int &id, const UserTool &tool) {
        try {
            db << "UPDATE tools SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "description = ?, name = ?, schema = ?, url = ? "
                  "WHERE id = ?"
               << tool.user_id
               << tool.folder_id
               << tool.created_at
               << tool.updated_at
               << tool.sharing
               << tool.description
               << tool.name
               << tool.schema.dump()
               << tool.url
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating tool: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating tool: " << e.what() << std::endl;
            return false;
        }
    }

    bool Tool::delete_tool(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM tools WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserTool> Tool::tools(const std::string &user_id) {
        std::vector<UserTool> result;

        try {
            db << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, description, name, schema, url "
                  "FROM tools WHERE user_id = ?;"
               << user_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string description, std::string name, std::string schema, std::string url) {
                   UserTool tool{
                           id, user_id, folder_id, created_at, updated_at, sharing, description, name, json::parse(schema), url};
                   result.push_back(tool);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving tools: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserTool> Tool::get_tools_by_workspace_id(const int &workspace_id) {
        std::vector<UserTool> result;

        try {
            db << "SELECT t.id, t.user_id, t.folder_id, t.created_at, t.updated_at, t.sharing, t.description, "
                  "t.name, t.schema, t.url "
                  "FROM tools t "
                  "JOIN tool_workspaces tw ON t.id = tw.tool_id "
                  "WHERE tw.workspace_id = ?;"
               << workspace_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string description, std::string name, std::string schema, std::string url) {
                   UserTool tool{
                           id, user_id, folder_id, created_at, updated_at, sharing, description, name, json::parse(schema), url};
                   result.push_back(tool);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving tools: " << e.what() << std::endl;
        }

        return result;
    }

    void Tool::to_json(json &j, const UserTool &tool) {
        j = json{
                {"id",                   tool.id},
                {"user_id",              tool.user_id},
                {"folder_id",            tool.folder_id},
                {"created_at",           tool.created_at},
                {"updated_at",           tool.updated_at},
                {"sharing",              tool.sharing},
                {"description",          tool.description},
                {"name",                 tool.name},
                {"schema",               tool.schema},
                {"url",                  tool.url}
        };
    };
}

#include "api/include/models/tool_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void ToolWorkspace::create_tool_workspace(const UserToolWorkspace &tool_workspace) {
        db << R"(INSERT INTO tool_workspaces (user_id, tool_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << tool_workspace.user_id
           << tool_workspace.tool_id
           << tool_workspace.workspace_id
           << tool_workspace.created_at
           << tool_workspace.updated_at;
    }

    UserToolWorkspace ToolWorkspace::get_tool_workspace_by_id(const int &tool_id, const int &workspace_id) {
        UserToolWorkspace tool_workspace;
        db << "SELECT * FROM tool_workspaces WHERE tool_id = ? AND workspace_id = ?;"
           << tool_id << workspace_id >>
           [&](std::string user_id, int tool_id, int workspace_id, std::string created_at, std::string updated_at) {
               tool_workspace = {user_id, tool_id, workspace_id, created_at, updated_at};
           };
        return tool_workspace;
    }

    bool ToolWorkspace::update_tool_workspace(const int &tool_id, const int &workspace_id, const UserToolWorkspace &tool_workspace) {
        try {
            db << "UPDATE tool_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE tool_id = ? AND workspace_id = ?"
               << tool_workspace.user_id
               << tool_workspace.created_at
               << tool_workspace.updated_at
               << tool_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating tool_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating tool_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool ToolWorkspace::delete_tool_workspace(const int &tool_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM tool_workspaces WHERE tool_id = ? AND workspace_id = ?;"
           << tool_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserToolWorkspace> ToolWorkspace::tool_workspaces(const std::string &user_id) {
        std::vector<UserToolWorkspace> result;

        try {
            db << "SELECT user_id, tool_id, workspace_id, created_at, updated_at "
                  "FROM tool_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int tool_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserToolWorkspace tool_workspace{
                           user_id, tool_id, workspace_id, created_at, updated_at};
                   result.push_back(tool_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving tool_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void ToolWorkspace::to_json(json &j, const UserToolWorkspace &tool_workspace) {
        j = json{
                {"user_id",              tool_workspace.user_id},
                {"tool_id",              tool_workspace.tool_id},
                {"workspace_id",         tool_workspace.workspace_id},
                {"created_at",           tool_workspace.created_at},
                {"updated_at",           tool_workspace.updated_at}
        };
    };
}

#include "api/include/models/assistant_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void AssistantWorkspace::create_assistant_workspace(const UserAssistantWorkspace &user_assistant_workspace) {
        db << R"(INSERT INTO assistant_workspaces (user_id, assistant_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << user_assistant_workspace.user_id
           << user_assistant_workspace.assistant_id
           << user_assistant_workspace.workspace_id
           << user_assistant_workspace.created_at
           << user_assistant_workspace.updated_at;
    }

    UserAssistantWorkspace AssistantWorkspace::get_assistant_workspace_by_id(const int &assistant_id, const int &workspace_id) {
        UserAssistantWorkspace user_assistant_workspace;
        db << "SELECT * FROM assistant_workspaces WHERE assistant_id = ? AND workspace_id = ?;"
           << assistant_id << workspace_id >>
           [&](std::string user_id, int assistant_id, int workspace_id, std::string created_at, std::string updated_at) {
               user_assistant_workspace = {user_id, assistant_id, workspace_id, created_at, updated_at};
           };
        return user_assistant_workspace;
    }

    bool AssistantWorkspace::update_assistant_workspace(const int &assistant_id, const int &workspace_id, const UserAssistantWorkspace &user_assistant_workspace) {
        try {
            db << "UPDATE assistant_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE assistant_id = ? AND workspace_id = ?"
               << user_assistant_workspace.user_id
               << user_assistant_workspace.created_at
               << user_assistant_workspace.updated_at
               << assistant_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating assistant_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating assistant_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool AssistantWorkspace::delete_assistant_workspace(const int &assistant_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM assistant_workspaces WHERE assistant_id = ? AND workspace_id = ?;"
           << assistant_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserAssistantWorkspace> AssistantWorkspace::get_assistants_by_workspace_id(const int &workspace_id) {
        std::vector<UserAssistantWorkspace> result;

        try {
            db << "SELECT user_id, assistant_id, workspace_id, created_at, updated_at FROM assistant_workspaces WHERE workspace_id = ?;"
               << workspace_id
               >> [&](std::string user_id, int assistant_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserAssistantWorkspace assistant_workspace{
                           user_id, assistant_id, workspace_id, created_at, updated_at};
                   result.push_back(assistant_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistant_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserAssistantWorkspace> AssistantWorkspace::assistant_workspaces(const std::string &user_id) {
        std::vector<UserAssistantWorkspace> result;

        try {
            db << "SELECT user_id, assistant_id, workspace_id, created_at, updated_at FROM assistant_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int assistant_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserAssistantWorkspace assistant_workspace{
                           user_id, assistant_id, workspace_id, created_at, updated_at};
                   result.push_back(assistant_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistant_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void AssistantWorkspace::to_json(json &j, const UserAssistantWorkspace &assistant_workspace) {
        j = json{
                {"user_id",                        assistant_workspace.user_id},
                {"assistant_id",                   assistant_workspace.assistant_id},
                {"workspace_id",                   assistant_workspace.workspace_id},
                {"created_at",                     assistant_workspace.created_at},
                {"updated_at",                     assistant_workspace.updated_at}
        };
    };
}

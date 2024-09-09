#include "api/include/models/prompt_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void PromptWorkspace::create_prompt_workspace(const UserPromptWorkspace &prompt_workspace) {
        db << R"(INSERT INTO prompt_workspaces (user_id, prompt_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << prompt_workspace.user_id
           << prompt_workspace.prompt_id
           << prompt_workspace.workspace_id
           << prompt_workspace.created_at
           << prompt_workspace.updated_at;
    }

    UserPromptWorkspace PromptWorkspace::get_prompt_workspace_by_id(const int &prompt_id, const int &workspace_id) {
        UserPromptWorkspace prompt_workspace;
        db << "SELECT * FROM prompt_workspaces WHERE prompt_id = ? AND workspace_id = ?;"
           << prompt_id << workspace_id >>
           [&](std::string user_id, int prompt_id, int workspace_id, std::string created_at, std::string updated_at) {
               prompt_workspace = {user_id, prompt_id, workspace_id, created_at, updated_at};
           };
        return prompt_workspace;
    }

    bool PromptWorkspace::update_prompt_workspace(const int &prompt_id, const int &workspace_id, const UserPromptWorkspace &prompt_workspace) {
        try {
            db << "UPDATE prompt_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE prompt_id = ? AND workspace_id = ?"
               << prompt_workspace.user_id
               << prompt_workspace.created_at
               << prompt_workspace.updated_at
               << prompt_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating prompt_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating prompt_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool PromptWorkspace::delete_prompt_workspace(const int &prompt_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM prompt_workspaces WHERE prompt_id = ? AND workspace_id = ?;"
           << prompt_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserPromptWorkspace> PromptWorkspace::prompt_workspaces(const std::string &user_id) {
        std::vector<UserPromptWorkspace> result;

        try {
            db << "SELECT user_id, prompt_id, workspace_id, created_at, updated_at "
                  "FROM prompt_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int prompt_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserPromptWorkspace prompt_workspace{
                           user_id, prompt_id, workspace_id, created_at, updated_at};
                   result.push_back(prompt_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving prompt_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void PromptWorkspace::to_json(json &j, const UserPromptWorkspace  &prompt_workspace) {
        j = json{
                {"user_id",              prompt_workspace.user_id},
                {"prompt_id",            prompt_workspace.prompt_id},
                {"workspace_id",         prompt_workspace.workspace_id},
                {"created_at",           prompt_workspace.created_at},
                {"updated_at",           prompt_workspace.updated_at}
        };
    };
}

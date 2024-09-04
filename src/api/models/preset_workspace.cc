#include "api/include/models/preset_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void PresetWorkspace::create_preset_workspace(const UserPresetWorkspace &user_preset_workspace) {
        db << R"(INSERT INTO preset_workspaces (user_id, preset_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << user_preset_workspace.user_id
           << user_preset_workspace.preset_id
           << user_preset_workspace.workspace_id
           << user_preset_workspace.created_at
           << user_preset_workspace.updated_at;
    }

    UserPresetWorkspace PresetWorkspace::get_preset_workspace_by_id(const int &preset_id, const int &workspace_id) {
        UserPresetWorkspace user_preset_workspace;
        db << "SELECT * FROM preset_workspaces WHERE preset_id = ? AND workspace_id = ?;"
           << preset_id << workspace_id >>
           [&](std::string user_id, int preset_id, int workspace_id, std::string created_at, std::string updated_at) {
               user_preset_workspace = {user_id, preset_id, workspace_id, created_at, updated_at};
           };
        return user_preset_workspace;
    }

    bool PresetWorkspace::update_preset_workspace(const int &preset_id, const int &workspace_id, const UserPresetWorkspace &user_preset_workspace) {
        try {
            db << "UPDATE preset_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE preset_id = ? AND workspace_id = ?"
               << user_preset_workspace.user_id
               << user_preset_workspace.created_at
               << user_preset_workspace.updated_at
               << preset_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating preset_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating preset_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool PresetWorkspace::delete_preset_workspace(const int &preset_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM preset_workspaces WHERE preset_id = ? AND workspace_id = ?;"
           << preset_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;

        return is_deleted > 0;
    }

    std::vector<UserPresetWorkspace> PresetWorkspace::preset_workspaces(const std::string &user_id) {
        std::vector<UserPresetWorkspace> result;

        try {
            db << "SELECT user_id, preset_id, workspace_id, created_at, updated_at FROM preset_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int preset_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserPresetWorkspace preset_workspace{
                           user_id, preset_id, workspace_id, created_at, updated_at};
                   result.push_back(preset_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving preset_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void PresetWorkspace::to_json(json &j, const UserPresetWorkspace &preset_workspace) {
        j = json{
                {"user_id",                        preset_workspace.user_id},
                {"preset_id",                      preset_workspace.preset_id},
                {"workspace_id",                   preset_workspace.workspace_id},
                {"created_at",                     preset_workspace.created_at},
                {"updated_at",                     preset_workspace.updated_at}
        };
    };
}

#include "api/include/models/model_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void ModelWorkspace::create_model_workspace(const UserModelWorkspace &model_workspace) {
        db << R"(INSERT INTO model_workspaces (user_id, model_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << model_workspace.user_id
           << model_workspace.model_id
           << model_workspace.workspace_id
           << model_workspace.created_at
           << model_workspace.updated_at;
    }

    UserModelWorkspace ModelWorkspace::get_model_workspace_by_id(const int &model_id, const int &workspace_id) {
        UserModelWorkspace model_workspace;
        db << "SELECT * FROM model_workspaces WHERE model_id = ? AND workspace_id = ?;"
           << model_id << workspace_id >>
           [&](std::string user_id, int model_id, int workspace_id, std::string created_at, std::string updated_at) {
               model_workspace = {user_id, model_id, workspace_id, created_at, updated_at};
           };
        return model_workspace;
    }

    bool ModelWorkspace::update_model_workspace(const int &model_id, const int &workspace_id, const UserModelWorkspace &model_workspace) {
        try {
            db << "UPDATE model_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE model_id = ? AND workspace_id = ?"
               << model_workspace.user_id
               << model_workspace.created_at
               << model_workspace.updated_at
               << model_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating model_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating model_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool ModelWorkspace::delete_model_workspace(const int &model_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM model_workspaces WHERE model_id = ? AND workspace_id = ?;"
           << model_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserModelWorkspace> ModelWorkspace::model_workspaces(const std::string &user_id) {
        std::vector<UserModelWorkspace> result;

        try {
            db << "SELECT user_id, model_id, workspace_id, created_at, updated_at "
                  "FROM model_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int model_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserModelWorkspace model_workspace{
                           user_id, model_id, workspace_id, created_at, updated_at};
                   result.push_back(model_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving model_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void ModelWorkspace::to_json(json &j, const UserModelWorkspace &model_workspace) {
        j = json{
                {"user_id",              model_workspace.user_id},
                {"model_id",             model_workspace.model_id},
                {"workspace_id",         model_workspace.workspace_id},
                {"created_at",           model_workspace.created_at},
                {"updated_at",           model_workspace.updated_at}
        };
    };
}

#include "api/include/models/collection_workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void CollectionWorkspace::create_collection_workspace(const UserCollectionWorkspace &collection_workspace) {
        db << R"(INSERT INTO collection_workspaces (user_id, collection_id, workspace_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << collection_workspace.user_id
           << collection_workspace.collection_id
           << collection_workspace.workspace_id
           << collection_workspace.created_at
           << collection_workspace.updated_at;
    }

    UserCollectionWorkspace CollectionWorkspace::get_collection_workspace_by_id(const int &collection_id, const int &workspace_id) {
        UserCollectionWorkspace collection_workspace;
        db << "SELECT * FROM collection_workspaces WHERE collection_id = ? AND workspace_id = ?;"
           << collection_id << workspace_id >>
           [&](std::string user_id, int collection_id, int workspace_id, std::string created_at, std::string updated_at) {
               collection_workspace = {user_id, collection_id, workspace_id, created_at, updated_at};
           };
        return collection_workspace;
    }

    bool CollectionWorkspace::update_collection_workspace(const int &collection_id, const int &workspace_id, const UserCollectionWorkspace &collection_workspace) {
        try {
            db << "UPDATE collection_workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE collection_id = ? AND workspace_id = ?"
               << collection_workspace.user_id
               << collection_workspace.created_at
               << collection_workspace.updated_at
               << collection_id
               << workspace_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating collection_workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating collection_workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool CollectionWorkspace::delete_collection_workspace(const int &collection_id, const int &workspace_id) {
        bool is_deleted = false;
        db << "DELETE FROM collection_workspaces WHERE collection_id = ? AND workspace_id = ?;"
           << collection_id << workspace_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserCollectionWorkspace> CollectionWorkspace::collection_workspaces(const std::string &user_id) {
        std::vector<UserCollectionWorkspace> result;

        try {
            db << "SELECT user_id, collection_id, workspace_id, created_at, updated_at "
                  "FROM collection_workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int collection_id, int workspace_id, std::string created_at, std::string updated_at) {
                   UserCollectionWorkspace collection_workspace{
                           user_id, collection_id, workspace_id, created_at, updated_at};
                   result.push_back(collection_workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving collection_workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void CollectionWorkspace::to_json(json &j, const UserCollectionWorkspace &collection_workspace) {
        j = json{
                {"user_id",              collection_workspace.user_id},
                {"collection_id",        collection_workspace.collection_id},
                {"workspace_id",         collection_workspace.workspace_id},
                {"created_at",           collection_workspace.created_at},
                {"updated_at",           collection_workspace.updated_at}
        };
    };
}

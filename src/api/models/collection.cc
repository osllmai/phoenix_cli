#include "api/include/models/collection.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Collection::create_collection(const UserCollection &collection) {
        db << R"(INSERT INTO collections (user_id, folder_id, created_at, updated_at, sharing, description, name)
               VALUES (?, ?, ?, ?, ?, ?, ?);)"
           << collection.user_id
           << collection.folder_id
           << collection.created_at
           << collection.updated_at
           << collection.sharing
           << collection.description
           << collection.name;
    }

    UserCollection Collection::get_collection_by_id(const int &id) {
        UserCollection collection;
        db << "SELECT * FROM collections WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, std::string description, std::string name) {
               collection = {id, user_id, folder_id, created_at, updated_at, sharing, description, name};
           };
        return collection;
    }

    bool Collection::update_collection(const int &id, const UserCollection &collection) {
        try {
            db << "UPDATE collections SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "description = ?, name = ? "
                  "WHERE id = ?"
               << collection.user_id
               << collection.folder_id
               << collection.created_at
               << collection.updated_at
               << collection.sharing
               << collection.description
               << collection.name
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating collection: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating collection: " << e.what() << std::endl;
            return false;
        }
    }

    bool Collection::delete_collection(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM collections WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserCollection> Collection::collections(const std::string &user_id) {
        std::vector<UserCollection> result;

        try {
            db << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, description, name "
                  "FROM collections WHERE user_id = ?;"
               << user_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string description, std::string name) {
                   UserCollection collection{
                           id, user_id, folder_id, created_at, updated_at, sharing, description, name};
                   result.push_back(collection);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving collections: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserCollection> Collection::get_collections_by_workspace_id(const int &workspace_id) {
        std::vector<UserCollection> result;

        try {
            db << "SELECT collections.* "
                  "FROM collections "
                  "JOIN folders ON collections.folder_id = folders.id "
                  "WHERE folders.workspace_id = ?;"
               << workspace_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string description, std::string name) {
                   UserCollection collection{
                           id, user_id, folder_id, created_at, updated_at, sharing, description, name};
                   result.push_back(collection);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving collections by workspace: " << e.what() << std::endl;
        }


        return result;
    }

    void Collection::to_json(json &j, const UserCollection &collection) {
        j = json{
                {"id",                   collection.id},
                {"user_id",              collection.user_id},
                {"folder_id",            collection.folder_id},
                {"created_at",           collection.created_at},
                {"updated_at",           collection.updated_at},
                {"sharing",              collection.sharing},
                {"description",          collection.description},
                {"name",                 collection.name}
        };
    };
}

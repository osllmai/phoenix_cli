#include "api/include/models/assistant_collection.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void AssistantCollection::create_assistant_collection(const UserAssistantCollection &assistant_collection) {
        db << R"(INSERT INTO assistant_collections (user_id, assistant_id, collection_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << assistant_collection.user_id
           << assistant_collection.assistant_id
           << assistant_collection.collection_id
           << assistant_collection.created_at
           << assistant_collection.updated_at;
    }

    UserAssistantCollection AssistantCollection::get_assistant_collection_by_id(const int &assistant_id, const int &collection_id) {
        UserAssistantCollection assistant_collection;
        db << "SELECT * FROM assistant_collections WHERE assistant_id = ? AND collection_id = ?;"
           << assistant_id << collection_id >>
           [&](std::string user_id, int assistant_id, int collection_id, std::string created_at, std::string updated_at) {
               assistant_collection = {user_id, assistant_id, collection_id, created_at, updated_at};
           };
        return assistant_collection;
    }

    bool AssistantCollection::update_assistant_collection(const int &assistant_id, const int &collection_id, const UserAssistantCollection &assistant_collection) {
        try {
            db << "UPDATE assistant_collections SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE assistant_id = ? AND collection_id = ?"
               << assistant_collection.user_id
               << assistant_collection.created_at
               << assistant_collection.updated_at
               << assistant_id
               << collection_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating assistant_collection: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating assistant_collection: " << e.what() << std::endl;
            return false;
        }
    }

    bool AssistantCollection::delete_assistant_collection(const int &assistant_id, const int &collection_id) {
        bool is_deleted = false;
        db << "DELETE FROM assistant_collections WHERE assistant_id = ? AND collection_id = ?;"
           << assistant_id << collection_id >> [&] {
            is_deleted = true;
        };
        return is_deleted;
    }

    std::vector<UserAssistantCollection> AssistantCollection::assistant_collections(const std::string &user_id) {
        std::vector<UserAssistantCollection> result;

        try {
            db << "SELECT user_id, assistant_id, collection_id, created_at, updated_at "
                  "FROM assistant_collections WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int assistant_id, int collection_id, std::string created_at, std::string updated_at) {
                   UserAssistantCollection assistant_collection{
                           user_id, assistant_id, collection_id, created_at, updated_at};
                   result.push_back(assistant_collection);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistant_collections: " << e.what() << std::endl;
        }

        return result;
    }

    void AssistantCollection::to_json(json &j, const UserAssistantCollection &assistant_collection) {
        j = json{
                {"user_id",              assistant_collection.user_id},
                {"assistant_id",         assistant_collection.assistant_id},
                {"collection_id",        assistant_collection.collection_id},
                {"created_at",           assistant_collection.created_at},
                {"updated_at",           assistant_collection.updated_at}
        };
    };
}

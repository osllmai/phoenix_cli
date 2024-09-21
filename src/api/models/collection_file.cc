#include "api/include/models/collection_file.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void CollectionFile::create_collection_file(const UserCollectionFile &collection_file) {
        db << R"(INSERT INTO collection_files (user_id, collection_id, file_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << collection_file.user_id
           << collection_file.collection_id
           << collection_file.file_id
           << collection_file.created_at
           << collection_file.updated_at;
    }

    UserCollectionFile CollectionFile::get_collection_file_by_id(const int &collection_id, const int &file_id) {
        UserCollectionFile collection_file;
        db << "SELECT * FROM collection_files WHERE collection_id = ? AND file_id = ?;"
           << collection_id << file_id >>
           [&](std::string user_id, int collection_id, int file_id, std::string created_at, std::string updated_at) {
               collection_file = {user_id, collection_id, file_id, created_at, updated_at};
           };
        return collection_file;
    }

    bool CollectionFile::update_collection_file(const int &collection_id, const int &file_id, const UserCollectionFile &collection_file) {
        try {
            db << "UPDATE collection_files SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE collection_id = ? AND file_id = ?"
               << collection_file.user_id
               << collection_file.created_at
               << collection_file.updated_at
               << collection_id
               << file_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating collection_file: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating collection_file: " << e.what() << std::endl;
            return false;
        }
    }

    bool CollectionFile::delete_collection_file(const int &collection_id, const int &file_id) {
        bool is_deleted = false;
        db << "DELETE FROM collection_files WHERE collection_id = ? AND file_id = ?;"
           << collection_id << file_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserCollectionFile> CollectionFile::collection_files(const std::string &user_id) {
        std::vector<UserCollectionFile> result;

        try {
            db << "SELECT user_id, collection_id, file_id, created_at, updated_at "
                  "FROM collection_files WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int collection_id, int file_id, std::string created_at, std::string updated_at) {
                   UserCollectionFile collection_file{
                           user_id, collection_id, file_id, created_at, updated_at};
                   result.push_back(collection_file);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving collection_files: " << e.what() << std::endl;
        }

        return result;
    }

    void CollectionFile::to_json(json &j, const UserCollectionFile &collection_file) {
        j = json{
                {"user_id",              collection_file.user_id},
                {"collection_id",        collection_file.collection_id},
                {"file_id",             collection_file.file_id},
                {"created_at",           collection_file.created_at},
                {"updated_at",           collection_file.updated_at}
        };
    };
}

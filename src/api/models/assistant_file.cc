#include "api/include/models/assistant_file.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void AssistantFile::create_assistant_file(const UserAssistantFile &assistant_file) {
        db << R"(INSERT INTO assistant_files (user_id, assistant_id, file_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << assistant_file.user_id
           << assistant_file.assistant_id
           << assistant_file.file_id
           << assistant_file.created_at
           << assistant_file.updated_at;
    }

    UserAssistantFile AssistantFile::get_assistant_file_by_id(const int &assistant_id, const int &file_id) {
        UserAssistantFile assistant_file;
        db << "SELECT * FROM assistant_files WHERE assistant_id = ? AND file_id = ?;"
           << assistant_id << file_id >>
           [&](std::string user_id, int assistant_id, int file_id, std::string created_at, std::string updated_at) {
               assistant_file = {user_id, assistant_id, file_id, created_at, updated_at};
           };
        return assistant_file;
    }

    bool AssistantFile::update_assistant_file(const int &assistant_id, const int &file_id, const UserAssistantFile &assistant_file) {
        try {
            db << "UPDATE assistant_files SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE assistant_id = ? AND file_id = ?"
               << assistant_file.user_id
               << assistant_file.created_at
               << assistant_file.updated_at
               << assistant_id
               << file_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating assistant_file: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating assistant_file: " << e.what() << std::endl;
            return false;
        }
    }

    bool AssistantFile::delete_assistant_file(const int &assistant_id, const int &file_id) {
        bool is_deleted = false;
        db << "DELETE FROM assistant_files WHERE assistant_id = ? AND file_id = ?;"
           << assistant_id << file_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserAssistantFile> AssistantFile::assistant_files(const std::string &user_id) {
        std::vector<UserAssistantFile> result;

        try {
            db << "SELECT user_id, assistant_id, file_id, created_at, updated_at "
                  "FROM assistant_files WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int assistant_id, int file_id, std::string created_at, std::string updated_at) {
                   UserAssistantFile assistant_file{
                           user_id, assistant_id, file_id, created_at, updated_at};
                   result.push_back(assistant_file);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistant_files: " << e.what() << std::endl;
        }

        return result;
    }

    void AssistantFile::to_json(json &j, const UserAssistantFile &assistant_file) {
        j = json{
                {"user_id",              assistant_file.user_id},
                {"assistant_id",         assistant_file.assistant_id},
                {"file_id",             assistant_file.file_id},
                {"created_at",           assistant_file.created_at},
                {"updated_at",           assistant_file.updated_at}
        };
    };
}

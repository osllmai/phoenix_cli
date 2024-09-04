#include "api/include/models/chat_file.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void ChatFile::create_chat_file(const UserChatFile &user_chat_file) {
        db << R"(INSERT INTO chat_files (user_id, chat_id, file_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << user_chat_file.user_id
           << user_chat_file.chat_id
           << user_chat_file.file_id
           << user_chat_file.created_at
           << user_chat_file.updated_at;
    }

    UserChatFile ChatFile::get_chat_file_by_id(const int &chat_id, const int &file_id) {
        UserChatFile user_chat_file;
        db << "SELECT * FROM chat_files WHERE chat_id = ? AND file_id = ?;"
           << chat_id << file_id >>
           [&](std::string user_id, int chat_id, int file_id, std::string created_at, std::string updated_at) {
               user_chat_file = {user_id, chat_id, file_id, created_at, updated_at};
           };
        return user_chat_file;
    }

    bool ChatFile::update_chat_file(const int &chat_id, const int &file_id, const UserChatFile &user_chat_file) {
        try {
            db << "UPDATE chat_files SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE chat_id = ? AND file_id = ?"
               << user_chat_file.user_id
               << user_chat_file.created_at
               << user_chat_file.updated_at
               << chat_id
               << file_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating chat_file: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating chat_file: " << e.what() << std::endl;
            return false;
        }
    }

    bool ChatFile::delete_chat_file(const int &chat_id, const int &file_id) {
        bool is_deleted = false;
        db << "DELETE FROM chat_files WHERE chat_id = ? AND file_id = ?;"
           << chat_id << file_id;

        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserChatFile> ChatFile::chat_files(const std::string &user_id) {
        std::vector<UserChatFile> result;

        try {
            db << "SELECT user_id, chat_id, file_id, created_at, updated_at FROM chat_files WHERE user_id = ?;"
               << user_id
               >> [&](std::string user_id, int chat_id, int file_id, std::string created_at, std::string updated_at) {
                   UserChatFile chat_file{
                           user_id, chat_id, file_id, created_at, updated_at};
                   result.push_back(chat_file);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving chat_files: " << e.what() << std::endl;
        }

        return result;
    }

    void ChatFile::to_json(json &j, const UserChatFile &chat_file) {
        j = json{
                {"user_id",                        chat_file.user_id},
                {"chat_id",                        chat_file.chat_id},
                {"file_id",                        chat_file.file_id},
                {"created_at",                     chat_file.created_at},
                {"updated_at",                     chat_file.updated_at}
        };
    };
}

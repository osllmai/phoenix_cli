#include "api/include/models/message.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Message::create_message(const UserMessage &user_message) {
        db << R"(INSERT INTO messages (chat_id, user_id, created_at, updated_at, content, image_paths, model, role, sequence_number)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << user_message.chat_id
           << user_message.user_id
           << user_message.created_at
           << user_message.updated_at
           << user_message.content
           << user_message.image_paths
           << user_message.model
           << user_message.role
           << user_message.sequence_number;
    }

    UserMessage Message::get_message_by_id(const int &id) {
        UserMessage user_message;
        db << "SELECT * FROM messages WHERE id = ?;" << id >>
           [&](int id, int chat_id, std::string user_id, std::string created_at, std::string updated_at,
               std::string content, std::vector<std::string> image_paths, std::string model, std::string role, int sequence_number) {
               user_message = {id, chat_id, user_id, created_at, updated_at, content, image_paths, model, role, sequence_number};
           };
        return user_message;
    }

    bool Message::update_message(const int &id, const UserMessage &user_message) {
        try {
            db << "UPDATE messages SET "
                  "chat_id = ?, user_id = ?, created_at = ?, updated_at = ?, content = ?, image_paths = ?, model = ?, role = ?, sequence_number = ? "
                  "WHERE id = ?"
               << user_message.chat_id
               << user_message.user_id
               << user_message.created_at
               << user_message.updated_at
               << user_message.content
               << user_message.image_paths
               << user_message.model
               << user_message.role
               << user_message.sequence_number
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating message: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating message: " << e.what() << std::endl;
            return false;
        }
    }

    bool Message::delete_message(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM messages WHERE id = ?;" << id >> [&] {
            is_deleted = true;
        };
        return is_deleted;
    }

    std::vector<UserMessage> Message::messages(const std::string &user_id) {
        std::vector<UserMessage> result;

        try {
            db << "SELECT id, chat_id, user_id, created_at, updated_at, content, image_paths, model, role, sequence_number "
                  "FROM messages WHERE user_id = ?;"
               << user_id
               >> [&](int id, int chat_id, std::string user_id, std::string created_at, std::string updated_at,
                      std::string content, std::vector<std::string> image_paths, std::string model, std::string role, int sequence_number) {
                   UserMessage message{
                           id, chat_id, user_id, created_at, updated_at, content, image_paths, model, role, sequence_number};
                   result.push_back(message);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving messages: " << e.what() << std::endl;
        }

        return result;
    }

    void Message::to_json(json &j, const UserMessage &message) {
        j = json{
                {"id",                    message.id},
                {"chat_id",               message.chat_id},
                {"user_id",              message.user_id},
                {"created_at",           message.created_at},
                {"updated_at",           message.updated_at},
                {"content",              message.content},
                {"image_paths",          message.image_paths},
                {"model",                message.model},
                {"role",                 message.role},
                {"sequence_number",      message.sequence_number}
        };
    };
}

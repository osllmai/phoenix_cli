#include "api/include/models/message_file_item.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void MessageFileItem::create_message_file_item(const UserMessageFileItem &user_message_file_item) {
        db << R"(INSERT INTO message_file_items (user_id, message_id, file_item_id, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?);)"
           << user_message_file_item.user_id
           << user_message_file_item.message_id
           << user_message_file_item.file_item_id
           << user_message_file_item.created_at
           << user_message_file_item.updated_at;
    }

    UserMessageFileItem MessageFileItem::get_message_file_item_by_id(const int &message_id, const int &file_item_id) {
        UserMessageFileItem user_message_file_item;
        db << "SELECT * FROM message_file_items WHERE message_id = ? AND file_item_id = ?;"
           << message_id << file_item_id >>
           [&](std::string user_id, int message_id, int file_item_id, std::string created_at, std::string updated_at) {
               user_message_file_item = {user_id, message_id, file_item_id, created_at, updated_at};
           };
        return user_message_file_item;
    }

    bool MessageFileItem::update_message_file_item(const int &message_id, const int &file_item_id,
                                                   const UserMessageFileItem &user_message_file_item) {
        try {
            db << "UPDATE message_file_items SET "
                  "user_id = ?, created_at = ?, updated_at = ? "
                  "WHERE message_id = ? AND file_item_id = ?"
               << user_message_file_item.user_id
               << user_message_file_item.created_at
               << user_message_file_item.updated_at
               << message_id
               << file_item_id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating message_file_item: " << e.what() << " (" << e.get_code() << ")"
                      << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating message_file_item: " << e.what() << std::endl;
            return false;
        }
    }

    bool MessageFileItem::delete_message_file_item(const int &message_id, const int &file_item_id) {
        bool is_deleted = false;
        db << "DELETE FROM message_file_items WHERE message_id = ? AND file_item_id = ?;"
           << message_id << file_item_id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserMessageFileItem> MessageFileItem::message_file_items(const std::string &user_id) {
        std::vector<UserMessageFileItem> result;

        try {
            db
                    << "SELECT user_id, message_id, file_item_id, created_at, updated_at FROM message_file_items WHERE user_id = ?;"
                    << user_id
                    >> [&](std::string user_id, int message_id, int file_item_id, std::string created_at,
                           std::string updated_at) {
                        UserMessageFileItem message_file_item{
                                user_id, message_id, file_item_id, created_at, updated_at};
                        result.push_back(message_file_item);
                    };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving message_file_items: " << e.what() << std::endl;
        }

        return result;
    }

    void MessageFileItem::to_json(json &j, const UserMessageFileItem &message_file_item) {
        j = json{
                {"user_id",      message_file_item.user_id},
                {"message_id",   message_file_item.message_id},
                {"file_item_id", message_file_item.file_item_id},
                {"created_at",   message_file_item.created_at},
                {"updated_at",   message_file_item.updated_at}
        };
    };
}

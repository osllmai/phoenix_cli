#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserMessageFileItem {
    std::string user_id;
    int message_id;
    int file_item_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class MessageFileItem {
    public:
        static UserMessageFileItem get_message_file_item_by_id(const int &message_id, const int &file_item_id);

        static void create_message_file_item(const UserMessageFileItem &user_message_file_item);

        static bool update_message_file_item(const int &message_id, const int &file_item_id, const UserMessageFileItem &user_message_file_item);

        static bool delete_message_file_item(const int &message_id, const int &file_item_id);

        static std::vector<UserMessageFileItem> message_file_items(const std::string &user_id);

        static void to_json(json &j, const UserMessageFileItem &message_file_item);
    };
}

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserMessage {
    int id;
    int chat_id;
    std::string user_id;
    std::string created_at;
    std::string updated_at;
    std::string content;
    std::vector<std::string> image_paths;
    std::string model;
    std::string role;
    int sequence_number;
};

namespace models {
    class Message {
    public:
        static UserMessage get_message_by_id(const int &id);

        static void create_message(const UserMessage &user_message);

        static bool update_message(const int &id, const UserMessage &user_message);

        static bool delete_message(const int &id);

        static std::vector<UserMessage> messages(const std::string &user_id);

        static void to_json(json &j, const UserMessage &message);
    };
}

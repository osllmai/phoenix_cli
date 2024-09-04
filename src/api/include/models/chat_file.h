#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserChatFile {
    std::string user_id;
    int chat_id;
    int file_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class ChatFile {
    public:
        static UserChatFile get_chat_file_by_id(const int &chat_id, const int &file_id);

        static void create_chat_file(const UserChatFile &user_chat_file);

        static bool update_chat_file(const int &chat_id, const int &file_id, const UserChatFile &user_chat_file);

        static bool delete_chat_file(const int &chat_id, const int &file_id);

        static std::vector<UserChatFile> chat_files(const std::string &user_id);

        static void to_json(json &j, const UserChatFile &chat_file);
    };
}

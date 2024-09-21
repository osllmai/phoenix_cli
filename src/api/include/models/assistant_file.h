#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserAssistantFile {
    std::string user_id;
    int assistant_id;
    int file_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class AssistantFile {
    public:
        static UserAssistantFile get_assistant_file_by_id(const int &assistant_id, const int &file_id);
        static void create_assistant_file(const UserAssistantFile &assistant_file);
        static bool update_assistant_file(const int &assistant_id, const int &file_id, const UserAssistantFile &assistant_file);
        static bool delete_assistant_file(const int &assistant_id, const int &file_id);
        static std::vector<UserAssistantFile> assistant_files(const std::string &user_id);
        static void to_json(json &j, const UserAssistantFile &assistant_file);
    };
}

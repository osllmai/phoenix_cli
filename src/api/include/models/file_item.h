#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserFileItem {
    int id;
    int file_id;
    std::string user_id;
    std::string sharing;
    std::string content;
    std::vector<float> local_embedding;
    std::vector<float> openai_embedding;
    int tokens;
    std::string created_at;
    std::string updated_at;

//    void to_json(nlohmann::json &j, const UserFileItem &user_file_item) {
//        j = json{
//                {"id",               user_file_item.id},
//                {"file_id",          user_file_item.file_id},
//                {"user_id",          user_file_item.user_id},
//                {"sharing",          user_file_item.sharing},
//                {"content",          user_file_item.content},
//                {"local_embedding",  user_file_item.local_embedding},
//                {"openai_embedding", user_file_item.openai_embedding},
//                {"tokens",           user_file_item.tokens},
//                {"created_at",       user_file_item.created_at},
//                {"updated_at",       user_file_item.updated_at}};
//    }
};

namespace models {
    class FileItem {
    public:
        static void create_file_item(const UserFileItem &user_file_item);

        static bool update_file_item(const int &file_item_id, const UserFileItem &user_file_item);

        static bool delete_file_item(const int &file_item_id);

        static UserFileItem get_file_item_by_id(const int &file_item_id);

        static void to_json(json &j, const UserFileItem &user_file_item);

        static std::vector<UserFileItem> file_items_by_file(const int &file_id);

        static std::vector<UserFileItem>
        match_file_items_local(const std::vector<float> &query_embedding, const int &match_count,
                               const std::vector<int> &file_ids);


    };
}
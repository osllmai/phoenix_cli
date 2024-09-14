#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserPrompt {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    std::string content;
    std::string name;
};

namespace models {
    class Prompt {
    public:
        static UserPrompt get_prompt_by_id(const int &id);

        static void create_prompt(const UserPrompt &user_prompt);

        static bool update_prompt(const int &id, const UserPrompt &user_prompt);

        static bool delete_prompt(const int &id);

        static std::vector<UserPrompt> prompts(const std::string &user_id);

        static std::vector<UserPrompt> get_prompts_by_workspace_id(const int &workspace_id);

        static void to_json(json &j, const UserPrompt &prompt);
    };
}


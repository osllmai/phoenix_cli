#include "api/include/models/prompt.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Prompt::create_prompt(const UserPrompt &user_prompt) {
        db << R"(INSERT INTO prompts (user_id, folder_id, created_at, updated_at, sharing, content, name)
               VALUES (?, ?, ?, ?, ?, ?, ?);)"
           << user_prompt.user_id
           << user_prompt.folder_id
           << user_prompt.created_at
           << user_prompt.updated_at
           << user_prompt.sharing
           << user_prompt.content
           << user_prompt.name;
    }

    UserPrompt Prompt::get_prompt_by_id(const int &id) {
        UserPrompt user_prompt;
        db << "SELECT * FROM prompts WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, std::string content, std::string name) {
               user_prompt = {id, user_id, folder_id, created_at, updated_at, sharing, content, name};
           };
        return user_prompt;
    }

    bool Prompt::update_prompt(const int &id, const UserPrompt &user_prompt) {
        try {
            db << "UPDATE prompts SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, content = ?, name = ? "
                  "WHERE id = ?"
               << user_prompt.user_id
               << user_prompt.folder_id
               << user_prompt.created_at
               << user_prompt.updated_at
               << user_prompt.sharing
               << user_prompt.content
               << user_prompt.name
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception& e) {
            std::cerr << "SQLite error updating prompt: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating prompt: " << e.what() << std::endl;
            return false;
        }
    }

    bool Prompt::delete_prompt(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM prompts WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserPrompt> Prompt::prompts(const std::string &user_id) {
        std::vector<UserPrompt> result;

        try {
            db << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, content, name "
                  "FROM prompts WHERE user_id = ?;"
               << user_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string content, std::string name) {
                   UserPrompt prompt{
                           id, user_id, folder_id, created_at, updated_at, sharing, content, name};
                   result.push_back(prompt);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving prompts: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserPrompt> Prompt::get_prompts_by_workspace_id(const int &workspace_id) {
        std::vector<UserPrompt> result;

        try {
            db << "SELECT p.id, p.user_id, p.folder_id, p.created_at, p.updated_at, p.sharing, p.name, p.content "
                  "FROM prompts p "
                  "JOIN prompt_workspaces pw ON p.id = pw.prompt_id "
                  "JOIN folders f ON pw.folder_id = f.id "
                  "WHERE f.workspace_id = ?;"
               << workspace_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string content, std::string name) {
                   UserPrompt prompt{
                           id, user_id, folder_id, created_at, updated_at, sharing, content, name};
                   result.push_back(prompt);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving prompts: " << e.what() << std::endl;
        }

        return result;
    }

    void Prompt::to_json(json &j, const UserPrompt &prompt) {
        j = json{
                {"id",                    prompt.id},
                {"user_id",               prompt.user_id},
                {"folder_id",             prompt.folder_id},
                {"created_at",           prompt.created_at},
                {"updated_at",           prompt.updated_at},
                {"sharing",              prompt.sharing},
                {"content",              prompt.content},
                {"name",                 prompt.name}
        };
    };
}


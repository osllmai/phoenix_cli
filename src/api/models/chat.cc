#include "api/include/models/chat.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Chat::create_chat(const UserChat &user_chat) {
        db << R"(INSERT INTO chats (user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing, context_length,
               description, embeddings_provider, include_profile_context, include_workspace_instructions, model, name, prompt, temperature)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << user_chat.user_id
           << user_chat.workspace_id
           << user_chat.assistant_id
           << user_chat.folder_id
           << user_chat.created_at
           << user_chat.updated_at
           << user_chat.sharing
           << user_chat.context_length
           << user_chat.description
           << user_chat.embeddings_provider
           << static_cast<int>(user_chat.include_profile_context)
           << static_cast<int>(user_chat.include_workspace_instructions)
           << user_chat.model
           << user_chat.name
           << user_chat.prompt
           << user_chat.temperature;
    }

    UserChat Chat::get_chat_by_id(const int &id) {
        UserChat user_chat;
        db << "SELECT * FROM chats WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int workspace_id, int assistant_id, int folder_id, std::string created_at,
               std::string updated_at,
               std::string sharing, int context_length, std::string description, std::string embeddings_provider,
               int include_profile_context, int include_workspace_instructions,
               std::string model, std::string name, std::string prompt, double temperature) {
               user_chat = {id, user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing,
                            context_length, description,
                            embeddings_provider, static_cast<bool>(include_profile_context),
                            static_cast<bool>(include_workspace_instructions),
                            model, name, prompt, temperature};
           };
        return user_chat;
    }

    bool Chat::update_chat(const int &id, const UserChat &user_chat) {
        try {
            db << "UPDATE chats SET "
                  "user_id = ?, workspace_id = ?, assistant_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "context_length = ?, description = ?, embeddings_provider = ?, include_profile_context = ?, include_workspace_instructions = ?, "
                  "model = ?, name = ?, prompt = ?, temperature = ? "
                  "WHERE id = ?"
               << user_chat.user_id
               << user_chat.workspace_id
               << user_chat.assistant_id
               << user_chat.folder_id
               << user_chat.created_at
               << user_chat.updated_at
               << user_chat.sharing
               << user_chat.context_length
               << user_chat.description
               << user_chat.embeddings_provider
               << user_chat.include_profile_context
               << user_chat.include_workspace_instructions
               << user_chat.model
               << user_chat.name
               << user_chat.prompt
               << user_chat.temperature
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating chat: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating chat: " << e.what() << std::endl;
            return false;
        }
    }

    bool Chat::delete_chat(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM chats WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserChat> Chat::chats(const std::string &user_id) {
        std::vector<UserChat> result;

        try {
            db
                    << "SELECT id, user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing, context_length, "
                       "description, embeddings_provider, include_profile_context, include_workspace_instructions, model, name, prompt, temperature "
                       "FROM chats WHERE user_id = ?;"
                    << user_id
                    >> [&](int id, std::string user_id, int workspace_id, int assistant_id, int folder_id,
                           std::string created_at, std::string updated_at,
                           std::string sharing, int context_length, std::string description,
                           std::string embeddings_provider,
                           int include_profile_context, int include_workspace_instructions,
                           std::string model, std::string name, std::string prompt, double temperature) {
                        UserChat chat{
                                id, user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing,
                                context_length,
                                description,
                                embeddings_provider, static_cast<bool>(include_profile_context),
                                static_cast<bool>(include_workspace_instructions),
                                model, name, prompt, temperature};
                        result.push_back(chat);
                    };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving chats: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserChat> Chat::get_chats_by_workspace_id(const int &workspace_id) {
        std::vector<UserChat> result;

        try {
            db
                    << "SELECT id, user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing, context_length, "
                       "description, embeddings_provider, include_profile_context, include_workspace_instructions, model, name, prompt, temperature "
                       "FROM chats WHERE workspace_id = ?;"
                    << workspace_id
                    >> [&](int id, std::string user_id, int workspace_id, int assistant_id, int folder_id,
                           std::string created_at, std::string updated_at,
                           std::string sharing, int context_length, std::string description,
                           std::string embeddings_provider,
                           int include_profile_context, int include_workspace_instructions,
                           std::string model, std::string name, std::string prompt, double temperature) {
                        UserChat chat{
                                id, user_id, workspace_id, assistant_id, folder_id, created_at, updated_at, sharing,
                                context_length,
                                description,
                                embeddings_provider, static_cast<bool>(include_profile_context),
                                static_cast<bool>(include_workspace_instructions),
                                model, name, prompt, temperature};
                        result.push_back(chat);
                    };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving chats: " << e.what() << std::endl;
        }

        return result;
    }

    void Chat::to_json(json &j, const UserChat &chat) {
        j = json{
                {"id",                             chat.id},
                {"user_id",                        chat.user_id},
                {"workspace_id",                   chat.workspace_id},
                {"assistant_id",                   chat.assistant_id},
                {"folder_id",                      chat.folder_id},
                {"created_at",                     chat.created_at},
                {"updated_at",                     chat.updated_at},
                {"sharing",                        chat.sharing},
                {"context_length",                 chat.context_length},
                {"description",                    chat.description},
                {"embeddings_provider",            chat.embeddings_provider},
                {"include_profile_context",        chat.include_profile_context},
                {"include_workspace_instructions", chat.include_workspace_instructions},
                {"model",                          chat.model},
                {"name",                           chat.name},
                {"prompt",                         chat.prompt},
                {"temperature",                    chat.temperature}
        };
    };
}

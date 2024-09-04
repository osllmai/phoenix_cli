#include "api/include/models/assistant.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Assistant::create_assistant(const UserAssistant &user_assistant) {
        db << R"(INSERT INTO assistants (user_id, folder_id, created_at, updated_at, sharing, context_length, description,
               embeddings_provider, include_profile_context, include_workspace_instructions, model, name, image_path, prompt, temperature)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << user_assistant.user_id
           << user_assistant.folder_id
           << user_assistant.created_at
           << user_assistant.updated_at
           << user_assistant.sharing
           << user_assistant.context_length
           << user_assistant.description
           << user_assistant.embeddings_provider
           << static_cast<int>(user_assistant.include_profile_context)
           << static_cast<int>(user_assistant.include_workspace_instructions)
           << user_assistant.model
           << user_assistant.name
           << user_assistant.image_path
           << user_assistant.prompt
           << user_assistant.temperature;
    }

    UserAssistant Assistant::get_assistant_by_id(const int &id) {
        UserAssistant user_assistant;
        db << "SELECT * FROM assistants WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, int context_length, std::string description, std::string embeddings_provider,
               int include_profile_context, int include_workspace_instructions, std::string model, std::string name,
               std::string image_path, std::string prompt, double temperature) {
               user_assistant = {id, user_id, folder_id, created_at, updated_at, sharing, context_length, description,
                                 embeddings_provider, static_cast<bool>(include_profile_context),
                                 static_cast<bool>(include_workspace_instructions), model, name, image_path, prompt,
                                 temperature};
           };
        return user_assistant;
    }

    bool Assistant::update_assistant(const int &id, const UserAssistant &user_assistant) {
        try {
            db << "UPDATE assistants SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "context_length = ?, description = ?, embeddings_provider = ?, "
                  "include_profile_context = ?, include_workspace_instructions = ?, "
                  "model = ?, name = ?, image_path = ?, prompt = ?, temperature = ? "
                  "WHERE id = ?"
               << user_assistant.user_id
               << user_assistant.folder_id
               << user_assistant.created_at
               << user_assistant.updated_at
               << user_assistant.sharing
               << user_assistant.context_length
               << user_assistant.description
               << user_assistant.embeddings_provider
               << user_assistant.include_profile_context
               << user_assistant.include_workspace_instructions
               << user_assistant.model
               << user_assistant.name
               << user_assistant.image_path
               << user_assistant.prompt
               << user_assistant.temperature
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating assistant: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating assistant: " << e.what() << std::endl;
            return false;
        }
    }

    bool Assistant::delete_assistant(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM assistants WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserAssistant> Assistant::assistants(const std::string &user_id) {
        std::vector<UserAssistant> result;

        try {
            db << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, context_length, description, "
                  "embeddings_provider, include_profile_context, include_workspace_instructions, model, name, image_path, prompt, "
                  "temperature FROM assistants WHERE user_id = ?;"
               << user_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, int context_length, std::string description, std::string embeddings_provider,
                      int include_profile_context, int include_workspace_instructions, std::string model,
                      std::string name,
                      std::string image_path, std::string prompt, double temperature) {
                   UserAssistant assistant{
                           id, user_id, folder_id, created_at, updated_at, sharing, context_length, description,
                           embeddings_provider, static_cast<bool>(include_profile_context),
                           static_cast<bool>(include_workspace_instructions), model, name, image_path, prompt,
                           temperature};
                   result.push_back(assistant);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving assistants: " << e.what() << std::endl;
        }

        return result;
    }

    void Assistant::to_json(json &j, const UserAssistant &assistant) {
        j = json{
                {"id",                             assistant.id},
                {"user_id",                        assistant.user_id},
                {"folder_id",                      assistant.folder_id},
                {"created_at",                     assistant.created_at},
                {"updated_at",                     assistant.updated_at},
                {"sharing",                        assistant.sharing},
                {"context_length",                 assistant.context_length},
                {"description",                    assistant.description},
                {"embeddings_provider",            assistant.embeddings_provider},
                {"include_profile_context",        assistant.include_profile_context},
                {"include_workspace_instructions", assistant.include_workspace_instructions},
                {"model",                          assistant.model},
                {"name",                           assistant.name},
                {"image_path",                     assistant.image_path},
                {"prompt",                         assistant.prompt},
                {"temperature",                    assistant.temperature}
        };
    };
}

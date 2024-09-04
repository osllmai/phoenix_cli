#include "api/include/models/preset.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <iostream>

namespace models {
    void PresetModel::create_preset(const Preset &preset) {
        db << R"(INSERT INTO presets (user_id, folder_id, created_at, updated_at, sharing, context_length,
               description, embeddings_provider, include_profile_context, include_workspace_instructions,
               model, name, prompt, temperature)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << preset.user_id
           << preset.folder_id
           << preset.created_at
           << preset.updated_at
           << preset.sharing
           << preset.context_length
           << preset.description
           << preset.embeddings_provider
           << static_cast<int>(preset.include_profile_context)
           << static_cast<int>(preset.include_workspace_instructions)
           << preset.model
           << preset.name
           << preset.prompt
           << preset.temperature;
    }

    Preset PresetModel::get_preset_by_id(const int &id) {
        Preset preset;
        db << "SELECT * FROM presets WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, int context_length, std::string description, std::string embeddings_provider,
               int include_profile_context, int include_workspace_instructions, std::string model,
               std::string name, std::string prompt, double temperature) {
               preset = {id, user_id, folder_id, created_at, updated_at, sharing, context_length,
                         description, embeddings_provider, static_cast<bool>(include_profile_context),
                         static_cast<bool>(include_workspace_instructions), model, name, prompt, temperature};
           };
        return preset;
    }

    bool PresetModel::update_preset(const int &id, const Preset &preset) {
        try {
            db << "UPDATE presets SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "context_length = ?, description = ?, embeddings_provider = ?, "
                  "include_profile_context = ?, include_workspace_instructions = ?, "
                  "model = ?, name = ?, prompt = ?, temperature = ? "
                  "WHERE id = ?"
               << preset.user_id
               << preset.folder_id
               << preset.created_at
               << preset.updated_at
               << preset.sharing
               << preset.context_length
               << preset.description
               << preset.embeddings_provider
               << preset.include_profile_context
               << preset.include_workspace_instructions
               << preset.model
               << preset.name
               << preset.prompt
               << preset.temperature
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const std::exception &e) {
            std::cerr << "Error updating preset: " << e.what() << std::endl;
            return false;
        }
    }

    bool PresetModel::delete_preset(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM presets WHERE id = ?;" << id >> [&] {
            is_deleted = true;
        };
        return is_deleted;
    }

    std::vector<Preset> PresetModel::get_presets(const std::string &user_id) {
        std::vector<Preset> result;
        try {
            db << "SELECT * FROM presets WHERE user_id = ?;" << user_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, int context_length, std::string description, std::string embeddings_provider,
                      int include_profile_context, int include_workspace_instructions, std::string model,
                      std::string name, std::string prompt, double temperature) {
                   Preset preset = {id, user_id, folder_id, created_at, updated_at, sharing, context_length,
                                    description, embeddings_provider, static_cast<bool>(include_profile_context),
                                    static_cast<bool>(include_workspace_instructions), model, name, prompt, temperature};
                   result.push_back(preset);
               };
        } catch (const std::exception &e) {
            std::cerr << "Error retrieving presets: " << e.what() << std::endl;
        }
        return result;
    }

    void PresetModel::to_json(json &j, const Preset &preset) {
        j = json{
                {"id", preset.id},
                {"user_id", preset.user_id},
                {"folder_id", preset.folder_id},
                {"created_at", preset.created_at},
                {"updated_at", preset.updated_at},
                {"sharing", preset.sharing},
                {"context_length", preset.context_length},
                {"description", preset.description},
                {"embeddings_provider", preset.embeddings_provider},
                {"include_profile_context", preset.include_profile_context},
                {"include_workspace_instructions", preset.include_workspace_instructions},
                {"model", preset.model},
                {"name", preset.name},
                {"prompt", preset.prompt},
                {"temperature", preset.temperature}
        };
    }
}
#include "api/include/models/workspace.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>


namespace models {
    void Workspace::create_workspace(const UserWorkspace &user_workspace) {
        db << R"(INSERT INTO workspaces (user_id, created_at, updated_at, sharing, default_context_length, default_model,
               default_prompt, default_temperature, description, embeddings_provider, include_profile_context,
               include_workspace_instructions, instructions, is_home, name)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << user_workspace.user_id
           << user_workspace.created_at
           << user_workspace.updated_at
           << user_workspace.sharing
           << user_workspace.default_context_length
           << user_workspace.default_model
           << user_workspace.default_prompt
           << user_workspace.default_temperature
           << user_workspace.description
           << user_workspace.embeddings_provider
           << static_cast<int>(user_workspace.include_profile_context)
           << static_cast<int>(user_workspace.include_workspace_instructions)
           << user_workspace.instructions
           << static_cast<int>(user_workspace.is_home)
           << user_workspace.name;
    }

    UserWorkspace Workspace::get_workspace_by_id(const int &id) {
        UserWorkspace user_workspace;
        db << "SELECT * FROM workspaces WHERE id = ?;" << id >>
           [&](std::string id, std::string user_id, std::string created_at, std::string updated_at, std::string sharing,
               int default_context_length, std::string default_model, std::string default_prompt,
               double default_temperature,
               std::string description, std::string embeddings_provider, int include_profile_context,
               int include_workspace_instructions,
               std::string instructions, int is_home, std::string name) {
               user_workspace = {id, user_id, created_at, updated_at, sharing, default_context_length, default_model,
                                 default_prompt,
                                 default_temperature, description, embeddings_provider,
                                 static_cast<bool>(include_profile_context),
                                 static_cast<bool>(include_workspace_instructions),
                                 instructions, static_cast<bool>(is_home), name};
           };
        return user_workspace;
    }

    bool Workspace::update_workspace(const int &id, const UserWorkspace &user_workspace) {
        try {
            db << "UPDATE workspaces SET "
                  "user_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "default_context_length = ?, default_model = ?, default_prompt = ?, "
                  "default_temperature = ?, description = ?, embeddings_provider = ?, "
                  "include_profile_context = ?, include_workspace_instructions = ?, "
                  "instructions = ?, is_home = ?, name = ? "
                  "WHERE id = ?"
               << user_workspace.user_id
               << user_workspace.created_at
               << user_workspace.updated_at
               << user_workspace.sharing
               << user_workspace.default_context_length
               << user_workspace.default_model
               << user_workspace.default_prompt
               << user_workspace.default_temperature
               << user_workspace.description
               << user_workspace.embeddings_provider
               << user_workspace.include_profile_context
               << user_workspace.include_workspace_instructions
               << user_workspace.instructions
               << user_workspace.is_home
               << user_workspace.name
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool Workspace::delete_workspace(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM workspaces WHERE id = ?;" << id >> [&] {
            is_deleted = true;
        };
        return is_deleted;
    }


    std::vector<UserWorkspace> Workspace::workspaces(const std::string &user_id) {
        std::vector<UserWorkspace> result;

        try {
            db << "SELECT id, user_id, created_at, updated_at, sharing, "
                  "default_context_length, default_model, default_prompt, default_temperature, "
                  "description, embeddings_provider, include_profile_context, "
                  "include_workspace_instructions, instructions, is_home, name "
                  "FROM workspaces WHERE user_id = ?;"
               << user_id
               >> [&](std::string id, std::string user_id, std::string created_at, std::string updated_at,
                      std::string sharing, int default_context_length, std::string default_model,
                      std::string default_prompt, double default_temperature, std::string description,
                      std::string embeddings_provider, int include_profile_context, int include_workspace_instructions,
                      std::string instructions, int is_home, std::string name) {
                   UserWorkspace workspace{
                           id, user_id, created_at, updated_at, sharing, default_context_length, default_model,
                           default_prompt, default_temperature, description, embeddings_provider,
                           static_cast<bool>(include_profile_context),
                           static_cast<bool>(include_workspace_instructions),
                           instructions, static_cast<bool>(is_home), name};
                   result.push_back(workspace);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving workspaces: " << e.what() << std::endl;
        }

        return result;
    }

    void Workspace::disable_home_workspaces() {
        try {
            db << "UPDATE workspaces SET is_home = 0;";
        } catch (const std::exception &e) {
            std::cerr << "Error retrieving workspaces: " << e.what() << std::endl;
        }
    }

    void Workspace::to_json(json &j, const UserWorkspace &workspace) {
        j = json{
                {"id",                             workspace.id},
                {"user_id",                        workspace.user_id},
                {"created_at",                     workspace.created_at},
                {"updated_at",                     workspace.updated_at},
                {"sharing",                        workspace.sharing},
                {"default_context_length",         workspace.default_context_length},
                {"default_model",                  workspace.default_model},
                {"default_prompt",                 workspace.default_prompt},
                {"default_temperature",            workspace.default_temperature},
                {"description",                    workspace.description},
                {"embeddings_provider",            workspace.embeddings_provider},
                {"include_profile_context",        workspace.include_profile_context},
                {"include_workspace_instructions", workspace.include_workspace_instructions},
                {"instructions",                   workspace.instructions},
                {"is_home",                        workspace.is_home},
                {"name",                           workspace.name}
        };
    };

    UserWorkspace Workspace::is_home_user_workspace(const std::string &user_id) {
        UserWorkspace user_workspace;
        db << "SELECT * FROM workspaces WHERE user_id = ? AND is_home = 1;" << user_id >>
           [&](std::string id, std::string user_id, std::string created_at, std::string updated_at, std::string sharing,
               int default_context_length, std::string default_model, std::string default_prompt,
               double default_temperature,
               std::string description, std::string embeddings_provider, int include_profile_context,
               int include_workspace_instructions,
               std::string instructions, int is_home, std::string name) {
               user_workspace = {id, user_id, created_at, updated_at, sharing, default_context_length, default_model,
                                 default_prompt,
                                 default_temperature, description, embeddings_provider,
                                 static_cast<bool>(include_profile_context),
                                 static_cast<bool>(include_workspace_instructions),
                                 instructions, static_cast<bool>(is_home), name};
           };
        return user_workspace;
    }


}
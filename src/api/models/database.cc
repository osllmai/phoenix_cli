#include "api/include/models/database.h"
#include "directory_manager.h"

#include <sqlite_modern_cpp.h>
#include <iostream>

namespace models {
    std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    sqlite::database db(db_path);

    void init_db() {
        try {
            // Create users table
            db << "CREATE TABLE IF NOT EXISTS users ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "email TEXT UNIQUE NOT NULL,"
                  "password TEXT NOT NULL,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT"
                  ");";

            // Create profiles table
            db << "CREATE TABLE IF NOT EXISTS profiles ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id INTEGER NOT NULL UNIQUE REFERENCES users(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "bio TEXT NOT NULL CHECK (length(bio) <= 500),"
                  "has_onboarded BOOLEAN NOT NULL DEFAULT FALSE,"
                  "image_url TEXT NOT NULL CHECK (length(image_url) <= 1000),"
                  "image_path TEXT NOT NULL CHECK (length(image_path) <= 1000),"
                  "profile_context TEXT NOT NULL CHECK (length(profile_context) <= 1500),"
                  "display_name TEXT NOT NULL CHECK (length(display_name) <= 100),"
                  "use_azure_openai BOOLEAN NOT NULL,"
                  "username TEXT NOT NULL UNIQUE CHECK (length(username) >= 3 AND length(username) <= 25),"
                  "anthropic_api_key TEXT CHECK (length(anthropic_api_key) <= 1000),"
                  "azure_openai_35_turbo_id TEXT CHECK (length(azure_openai_35_turbo_id) <= 1000),"
                  "azure_openai_45_turbo_id TEXT CHECK (length(azure_openai_45_turbo_id) <= 1000),"
                  "azure_openai_45_vision_id TEXT CHECK (length(azure_openai_45_vision_id) <= 1000),"
                  "azure_openai_api_key TEXT CHECK (length(azure_openai_api_key) <= 1000),"
                  "azure_openai_endpoint TEXT CHECK (length(azure_openai_endpoint) <= 1000),"
                  "google_gemini_api_key TEXT CHECK (length(google_gemini_api_key) <= 1000),"
                  "mistral_api_key TEXT CHECK (length(mistral_api_key) <= 1000),"
                  "openai_api_key TEXT CHECK (length(openai_api_key) <= 1000),"
                  "openai_organization_id TEXT CHECK (length(openai_organization_id) <= 1000),"
                  "perplexity_api_key TEXT CHECK (length(perplexity_api_key) <= 1000)"
                  ");";

            // Create workspaces table
            db << "CREATE TABLE IF NOT EXISTS workspaces ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "default_context_length INTEGER NOT NULL,"
                  "default_model TEXT NOT NULL CHECK (length(default_model) <= 1000),"
                  "default_prompt TEXT NOT NULL CHECK (length(default_prompt) <= 100000),"
                  "default_temperature REAL NOT NULL,"
                  "description TEXT NOT NULL CHECK (length(description) <= 500),"
                  "embeddings_provider TEXT NOT NULL CHECK (length(embeddings_provider) <= 1000),"
                  "include_profile_context INTEGER NOT NULL,"
                  "include_workspace_instructions INTEGER NOT NULL,"
                  "instructions TEXT NOT NULL CHECK (length(instructions) <= 1500),"
                  "is_home INTEGER NOT NULL DEFAULT 0,"
                  "name TEXT NOT NULL CHECK (length(name) <= 100)"
                  ");";

            // Create indexes
            db << "CREATE INDEX IF NOT EXISTS idx_workspaces_user_id ON workspaces (user_id);";
            db << "CREATE UNIQUE INDEX IF NOT EXISTS idx_unique_home_workspace_per_user ON workspaces(user_id) WHERE is_home;";

            // Create folders table
            db << "CREATE TABLE IF NOT EXISTS folders ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "workspace_id INTEGER NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "name TEXT NOT NULL,"
                  "description TEXT NOT NULL,"
                  "type TEXT NOT NULL"
                  ");";

            // Create indexes for folders
            db << "CREATE INDEX IF NOT EXISTS idx_folders_user_id ON folders (user_id);";
            db << "CREATE INDEX IF NOT EXISTS idx_folders_workspace_id ON folders (workspace_id);";

            // Create files table
            db << "CREATE TABLE IF NOT EXISTS files ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "folder_id INTEGER REFERENCES folders(id) ON DELETE SET NULL,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "description TEXT NOT NULL CHECK (length(description) <= 500),"
                  "file_path TEXT NOT NULL CHECK (length(file_path) <= 1000),"
                  "name TEXT NOT NULL CHECK (length(name) <= 100),"
                  "size INTEGER NOT NULL,"
                  "tokens INTEGER NOT NULL,"
                  "type TEXT NOT NULL CHECK (length(type) <= 100)"
                  ");";

            // Create indexes for files
            db << "CREATE INDEX IF NOT EXISTS files_user_id_idx ON files(user_id);";

            // Create triggers for files
            db << "CREATE TRIGGER IF NOT EXISTS update_files_updated_at "
                  "BEFORE UPDATE ON files "
                  "FOR EACH ROW "
                  "BEGIN "
                  "UPDATE files SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id; "
                  "END;";

            db << "CREATE TRIGGER IF NOT EXISTS delete_old_file "
                  "BEFORE DELETE ON files "
                  "FOR EACH ROW "
                  "BEGIN "
                  "SELECT delete_storage_object_from_bucket('files', OLD.file_path); "
                  "END;";

            // Create file_items table
            db << "CREATE TABLE IF NOT EXISTS file_items ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "file_id INTEGER NOT NULL REFERENCES files(id) ON DELETE CASCADE,"
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "content TEXT NOT NULL,"
                  "local_embedding TEXT,"
                  "openai_embedding TEXT,"
                  "tokens INTEGER NOT NULL"
                  ");";

            // Create indexes for file_items
            db << "CREATE INDEX IF NOT EXISTS file_items_file_id_idx ON file_items(file_id);";

            // Create presets table
            db << "CREATE TABLE IF NOT EXISTS presets ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "folder_id INTEGER REFERENCES folders(id) ON DELETE SET NULL,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "context_length INTEGER NOT NULL,"
                  "description TEXT NOT NULL CHECK (length(description) <= 500),"
                  "embeddings_provider TEXT NOT NULL CHECK (length(embeddings_provider) <= 1000),"
                  "include_profile_context BOOLEAN NOT NULL,"
                  "include_workspace_instructions BOOLEAN NOT NULL,"
                  "model TEXT NOT NULL CHECK (length(model) <= 1000),"
                  "name TEXT NOT NULL CHECK (length(name) <= 100),"
                  "prompt TEXT NOT NULL CHECK (length(prompt) <= 100000),"
                  "temperature REAL NOT NULL"
                  ");";

            // Create indexes for presets
            db << "CREATE INDEX IF NOT EXISTS presets_user_id_idx ON presets(user_id);";

            // Create triggers for presets
            db << "CREATE TRIGGER IF NOT EXISTS update_presets_updated_at "
                  "BEFORE UPDATE ON presets "
                  "FOR EACH ROW "
                  "BEGIN "
                  "UPDATE presets SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id; "
                  "END;";

            // Create preset_workspaces table
            db << "CREATE TABLE IF NOT EXISTS preset_workspaces ("
                  "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "preset_id INTEGER NOT NULL REFERENCES presets(id) ON DELETE CASCADE,"
                  "workspace_id INTEGER NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "PRIMARY KEY(preset_id, workspace_id)"
                  ");";

            // Create indexes for preset_workspaces
            db << "CREATE INDEX IF NOT EXISTS preset_workspaces_user_id_idx ON preset_workspaces(user_id);";
            db << "CREATE INDEX IF NOT EXISTS preset_workspaces_preset_id_idx ON preset_workspaces(preset_id);";
            db << "CREATE INDEX IF NOT EXISTS preset_workspaces_workspace_id_idx ON preset_workspaces(workspace_id);";

            // Create triggers for preset_workspaces
            db << "CREATE TRIGGER IF NOT EXISTS update_preset_workspaces_updated_at "
                  "BEFORE UPDATE ON preset_workspaces "
                  "FOR EACH ROW "
                  "BEGIN "
                  "UPDATE preset_workspaces SET updated_at = CURRENT_TIMESTAMP WHERE user_id = OLD.user_id; "
                  "END;";

            // Create assistants table
            db << "CREATE TABLE IF NOT EXISTS assistants ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "folder_id INTEGER REFERENCES folders(id) ON DELETE SET NULL,"
                  "created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at DATETIME,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "context_length INTEGER NOT NULL,"
                  "description TEXT NOT NULL CHECK (length(description) <= 500),"
                  "embeddings_provider TEXT NOT NULL CHECK (length(embeddings_provider) <= 1000),"
                  "include_profile_context BOOLEAN NOT NULL,"
                  "include_workspace_instructions BOOLEAN NOT NULL,"
                  "model TEXT NOT NULL CHECK (length(model) <= 1000),"
                  "name TEXT NOT NULL CHECK (length(name) <= 100),"
                  "image_path TEXT NOT NULL CHECK (length(image_path) <= 1000),"
                  "prompt TEXT NOT NULL CHECK (length(prompt) <= 100000),"
                  "temperature REAL NOT NULL"
                  ");";

            // Create indexes for assistants
            db << "CREATE INDEX IF NOT EXISTS assistants_user_id_idx ON assistants(user_id);";

            // Create assistant_workspaces table
            db << "CREATE TABLE IF NOT EXISTS assistant_workspaces ("
                  "user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "assistant_id INTEGER NOT NULL REFERENCES assistants(id) ON DELETE CASCADE,"
                  "workspace_id INTEGER NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,"
                  "created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at DATETIME,"
                  "PRIMARY KEY(assistant_id, workspace_id)"
                  ");";

            // Create indexes for assistant_workspaces
            db << "CREATE INDEX IF NOT EXISTS assistant_workspaces_user_id_idx ON assistant_workspaces(user_id);";
            db << "CREATE INDEX IF NOT EXISTS assistant_workspaces_assistant_id_idx ON assistant_workspaces(assistant_id);";
            db << "CREATE INDEX IF NOT EXISTS assistant_workspaces_workspace_id_idx ON assistant_workspaces(workspace_id);";

            // Create chats table
            db << "CREATE TABLE IF NOT EXISTS chats ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "workspace_id INTEGER NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,"
                  "assistant_id INTEGER REFERENCES assistants(id) ON DELETE CASCADE,"
                  "folder_id INTEGER REFERENCES folders(id) ON DELETE SET NULL,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "sharing TEXT NOT NULL DEFAULT 'private',"
                  "context_length INT NOT NULL,"
                  "description TEXT NOT NULL CHECK (length(description) <= 500),"
                  "embeddings_provider TEXT NOT NULL CHECK (length(embeddings_provider) <= 1000),"
                  "include_profile_context BOOLEAN NOT NULL,"
                  "include_workspace_instructions BOOLEAN NOT NULL,"
                  "model TEXT NOT NULL CHECK (length(model) <= 1000),"
                  "name TEXT NOT NULL CHECK (length(name) <= 200),"
                  "prompt TEXT NOT NULL CHECK (length(prompt) <= 100000),"
                  "temperature REAL NOT NULL"
                  ");";

            // Create indexes for chats
            db << "CREATE INDEX IF NOT EXISTS idx_chats_user_id ON chats (user_id);";
            db << "CREATE INDEX IF NOT EXISTS idx_chats_workspace_id ON chats (workspace_id);";

            db << "CREATE TABLE IF NOT EXISTS chat_files ("
                  "user_id INTEGER NOT NULL,"
                  "chat_id INTEGER NOT NULL,"
                  "file_id INTEGER NOT NULL,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "PRIMARY KEY(chat_id, file_id),"
                  "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,"
                  "FOREIGN KEY(chat_id) REFERENCES chats(id) ON DELETE CASCADE,"
                  "FOREIGN KEY(file_id) REFERENCES files(id) ON DELETE CASCADE"
                  ");";

            // Create indexes for chat_files
            db << "CREATE INDEX IF NOT EXISTS idx_chat_files_chat_id ON chat_files (chat_id);";


            std::cout << "Database initialized successfully." << std::endl;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "Database initialization failed: " << e.what() << std::endl;
            std::cerr << "Error code: " << e.get_code() << std::endl;
            exit(1);
        } catch (const std::exception &e) {
            std::cerr << "Database initialization failed: " << e.what() << std::endl;
            exit(1);
        }
    }
}

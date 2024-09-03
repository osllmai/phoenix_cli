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
                  "user_id TEXT NOT NULL UNIQUE REFERENCES users(id) ON DELETE CASCADE,"
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
                  "user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
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
                  "user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,"
                  "workspace_id TEXT NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,"
                  "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TEXT,"
                  "name TEXT NOT NULL,"
                  "description TEXT NOT NULL,"
                  "type TEXT NOT NULL"
                  ");";

            // Create indexes for folders
            db << "CREATE INDEX IF NOT EXISTS idx_folders_user_id ON folders (user_id);";
            db << "CREATE INDEX IF NOT EXISTS idx_folders_workspace_id ON folders (workspace_id);";



            std::cout << "Database initialized successfully." << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Database initialization failed: " << e.what() << std::endl;
            exit(1);
        }
    }
}

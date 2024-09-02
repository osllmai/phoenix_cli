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
                  "id TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),"
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

            std::cout << "Database initialized successfully." << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Database initialization failed: " << e.what() << std::endl;
            exit(1);
        }
    }
}
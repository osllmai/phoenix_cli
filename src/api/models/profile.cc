#include "api/include/models/profile.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>

namespace models {
    void Profile::create(const UserProfile &user_profile) {
        db << R"(INSERT INTO profiles
        (user_id, bio, has_onboarded, image_url, image_path, profile_context, display_name,
         use_azure_openai, username)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << user_profile.user_id << user_profile.bio << user_profile.has_onboarded << user_profile.image_url
           << user_profile.image_path << user_profile.profile_context << user_profile.display_name
           << user_profile.use_azure_openai << user_profile.username;
    }

    UserProfile Profile::get_by_id(const std::string &user_id) {
        UserProfile user_profile; // Default constructor

        db << "SELECT id, user_id, created_at, updated_at, bio, has_onboarded, image_url, image_path, "
              "profile_context, display_name, use_azure_openai, username, anthropic_api_key, "
              "azure_openai_35_turbo_id, azure_openai_45_turbo_id, azure_openai_45_vision_id, "
              "azure_openai_api_key, azure_openai_endpoint, google_gemini_api_key, mistral_api_key, "
              "openai_api_key, openai_organization_id, perplexity_api_key "
              "FROM profiles WHERE user_id = ?;"
           << user_id
           >> [&](std::string id, std::string user_id, std::string created_at, std::string updated_at,
                  std::string bio, int has_onboarded, std::string image_url, std::string image_path,
                  std::string profile_context, std::string display_name, int use_azure_openai,
                  std::string username, std::string anthropic_api_key, std::string azure_openai_35_turbo_id,
                  std::string azure_openai_45_turbo_id, std::string azure_openai_45_vision_id,
                  std::string azure_openai_api_key, std::string azure_openai_endpoint,
                  std::string google_gemini_api_key, std::string mistral_api_key,
                  std::string openai_api_key, std::string openai_organization_id,
                  std::string perplexity_api_key) {
               // Set fields individually
               user_profile.id = id;
               user_profile.user_id = user_id;
               user_profile.created_at = created_at;
               user_profile.updated_at = updated_at;
               user_profile.bio = bio;
               user_profile.has_onboarded = static_cast<bool>(has_onboarded);
               user_profile.image_url = image_url;
               user_profile.image_path = image_path;
               user_profile.profile_context = profile_context;
               user_profile.display_name = display_name;
               user_profile.use_azure_openai = static_cast<bool>(use_azure_openai);
               user_profile.username = username;
               user_profile.anthropic_api_key = anthropic_api_key;
               user_profile.azure_openai_35_turbo_id = azure_openai_35_turbo_id;
               user_profile.azure_openai_45_turbo_id = azure_openai_45_turbo_id;
               user_profile.azure_openai_45_vision_id = azure_openai_45_vision_id;
               user_profile.azure_openai_api_key = azure_openai_api_key;
               user_profile.azure_openai_endpoint = azure_openai_endpoint;
               user_profile.google_gemini_api_key = google_gemini_api_key;
               user_profile.mistral_api_key = mistral_api_key;
               user_profile.openai_api_key = openai_api_key;
               user_profile.openai_organization_id = openai_organization_id;
               user_profile.perplexity_api_key = perplexity_api_key;
           };

        return user_profile;
    }



    void Profile::update(const UserProfile &user_profile) {
        db << R"(UPDATE profiles SET bio = ?, has_onboarded = ?, image_url = ?, image_path = ?,
             profile_context = ?, display_name = ?, use_azure_openai = ?, username = ?,
             anthropic_api_key = ?, azure_openai_35_turbo_id = ?, azure_openai_45_turbo_id = ?,
             azure_openai_45_vision_id = ?, azure_openai_api_key = ?, azure_openai_endpoint = ?,
             google_gemini_api_key = ?, mistral_api_key = ?, openai_api_key = ?,
             openai_organization_id = ?, perplexity_api_key = ?, updated_at = CURRENT_TIMESTAMP
             WHERE user_id = ?;)"
           << user_profile.bio << user_profile.has_onboarded << user_profile.image_url << user_profile.image_path
           << user_profile.profile_context << user_profile.display_name << user_profile.use_azure_openai
           << user_profile.username << user_profile.anthropic_api_key << user_profile.azure_openai_35_turbo_id
           << user_profile.azure_openai_45_turbo_id << user_profile.azure_openai_45_vision_id
           << user_profile.azure_openai_api_key << user_profile.azure_openai_endpoint
           << user_profile.google_gemini_api_key << user_profile.mistral_api_key << user_profile.openai_api_key
           << user_profile.openai_organization_id << user_profile.perplexity_api_key << user_profile.user_id;

    }
}

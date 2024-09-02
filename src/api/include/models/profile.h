//
// Created by Amir Kouhkan on 9/2/24.
//

#pragma once

#include <string>

struct UserProfile {
    std::string id;
    std::string user_id;
    std::string created_at;
    std::string updated_at;
    std::string bio;
    bool has_onboarded;
    std::string image_url;
    std::string image_path;
    std::string profile_context;
    std::string display_name;
    bool use_azure_openai;
    std::string username;
    std::string anthropic_api_key;
    std::string azure_openai_35_turbo_id;
    std::string azure_openai_45_turbo_id;
    std::string azure_openai_45_vision_id;
    std::string azure_openai_api_key;
    std::string azure_openai_endpoint;
    std::string google_gemini_api_key;
    std::string mistral_api_key;
    std::string openai_api_key;
    std::string openai_organization_id;
    std::string perplexity_api_key;

    // Default constructor
    UserProfile()
            : id(""), user_id(""), created_at(""), updated_at(""), bio(""), has_onboarded(false),
              image_url(""), image_path(""), profile_context(""), display_name(""), use_azure_openai(false),
              username(""), anthropic_api_key(""), azure_openai_35_turbo_id(""), azure_openai_45_turbo_id(""),
              azure_openai_45_vision_id(""), azure_openai_api_key(""), azure_openai_endpoint(""),
              google_gemini_api_key(""), mistral_api_key(""), openai_api_key(""), openai_organization_id(""),
              perplexity_api_key("") {}

    // Constructor with all fields
    UserProfile(const std::string &id, const std::string &user_id, const std::string &created_at,
                const std::string &updated_at, const std::string &bio, bool has_onboarded,
                const std::string &image_url, const std::string &image_path, const std::string &profile_context,
                const std::string &display_name, bool use_azure_openai, const std::string &username,
                const std::string &anthropic_api_key = "", const std::string &azure_openai_35_turbo_id = "",
                const std::string &azure_openai_45_turbo_id = "", const std::string &azure_openai_45_vision_id = "",
                const std::string &azure_openai_api_key = "", const std::string &azure_openai_endpoint = "",
                const std::string &google_gemini_api_key = "", const std::string &mistral_api_key = "",
                const std::string &openai_api_key = "", const std::string &openai_organization_id = "",
                const std::string &perplexity_api_key = "")
            : id(id), user_id(user_id), created_at(created_at), updated_at(updated_at), bio(bio),
              has_onboarded(has_onboarded), image_url(image_url), image_path(image_path),
              profile_context(profile_context),
              display_name(display_name), use_azure_openai(use_azure_openai), username(username),
              anthropic_api_key(anthropic_api_key), azure_openai_35_turbo_id(azure_openai_35_turbo_id),
              azure_openai_45_turbo_id(azure_openai_45_turbo_id), azure_openai_45_vision_id(azure_openai_45_vision_id),
              azure_openai_api_key(azure_openai_api_key), azure_openai_endpoint(azure_openai_endpoint),
              google_gemini_api_key(google_gemini_api_key), mistral_api_key(mistral_api_key),
              openai_api_key(openai_api_key), openai_organization_id(openai_organization_id),
              perplexity_api_key(perplexity_api_key) {}

    // Constructor for convenience
    UserProfile(const std::string &user_id, const std::string &bio, const std::string &image_url,
                const std::string &image_path, const std::string &profile_context,
                const std::string &display_name, bool use_azure_openai, const std::string &username)
            : user_id(user_id), bio(bio), has_onboarded(false), image_url(image_url),
              image_path(image_path), profile_context(profile_context), display_name(display_name),
              use_azure_openai(use_azure_openai), username(username) {}
};


namespace models {
    class Profile {
    public:
        static void create(const UserProfile &user_profile);

        static UserProfile get_by_id(const std::string &user_id);

        static void update(const UserProfile &user_profile);
    };
}

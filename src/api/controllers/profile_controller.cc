#include "api/include/controllers/profile_controller.h"
#include "api/include/models/profile.h"

#include <crow.h>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

namespace controllers {
    crow::response update_profile(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body) {
                return crow::response(400, "Invalid JSON");
            }

            UserProfile user_profile = models::Profile::get_by_id(request_body["user_id"]);
            user_profile.bio = request_body["bio"].get<std::string>();
            user_profile.has_onboarded = request_body["has_onboarded"].get<bool>();
            user_profile.image_url = request_body["image_url"].get<std::string>();
            user_profile.image_path = request_body["image_path"].get<std::string>();
            user_profile.profile_context = request_body["profile_context"].get<std::string>();
            user_profile.display_name = request_body["display_name"].get<std::string>();
            user_profile.use_azure_openai = request_body["use_azure_openai"].get<bool>();
            user_profile.username = request_body["username"].get<std::string>();
            user_profile.anthropic_api_key = request_body["anthropic_api_key"].get<std::string>();
            user_profile.azure_openai_35_turbo_id = request_body["azure_openai_35_turbo_id"].get<std::string>();
            user_profile.azure_openai_45_turbo_id = request_body["azure_openai_45_turbo_id"].get<std::string>();
            user_profile.azure_openai_45_vision_id = request_body["azure_openai_45_vision_id"].get<std::string>();
            user_profile.azure_openai_api_key = request_body["azure_openai_api_key"].get<std::string>();
            user_profile.azure_openai_endpoint = request_body["azure_openai_endpoint"].get<std::string>();
            user_profile.google_gemini_api_key = request_body["google_gemini_api_key"].get<std::string>();
            user_profile.mistral_api_key = request_body["mistral_api_key"].get<std::string>();
            user_profile.openai_api_key = request_body["openai_api_key"].get<std::string>();
            user_profile.openai_organization_id = request_body["openai_organization_id"].get<std::string>();
            user_profile.perplexity_api_key = request_body["perplexity_api_key"].get<std::string>();
            models::Profile::update(user_profile);

            json response = {
                    {"message", "User profile updated successfully"}
            };
            return crow::response(200, response.dump());

        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error" + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "Exception during update profile: " << e.what();
            return crow::response(400, "Exception during update profile" + std::string(e.what()));
        }
    }

    crow::response get_by_user_id(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            UserProfile get_user_profile = models::Profile::get_by_id(request_body["user_id"].get<std::string>());

            if (get_user_profile.user_id.empty())
                return crow::response(404, "User Profile not found");

            json response;
            response["id"] = get_user_profile.id;
            response["user_id"] = get_user_profile.user_id;
            response["username"] = get_user_profile.username;
            response["anthropic_api_key"] = get_user_profile.anthropic_api_key;
            response["azure_openai_45_vision_id"] = get_user_profile.azure_openai_45_vision_id;
            response["azure_openai_45_turbo_id"] = get_user_profile.azure_openai_45_turbo_id;
            response["azure_openai_35_turbo_id"] = get_user_profile.azure_openai_35_turbo_id;
            response["azure_openai_api_key"] = get_user_profile.azure_openai_api_key;
            response["azure_openai_endpoint"] = get_user_profile.azure_openai_endpoint;
            response["use_azure_openai"] = get_user_profile.use_azure_openai;
            response["bio"] = get_user_profile.bio;
            response["has_onboarded"] = get_user_profile.has_onboarded;
            response["created_at"] = get_user_profile.created_at;
            response["updated_at"] = get_user_profile.updated_at;
            response["has_onboarded"] = get_user_profile.has_onboarded;
            response["image_path"] = get_user_profile.image_path;
            response["image_url"] = get_user_profile.image_url;
            response["profile_context"] = get_user_profile.profile_context;
            response["display_name"] = get_user_profile.display_name;
            response["google_gemini_api_key"] = get_user_profile.google_gemini_api_key;
            response["mistral_api_key"] = get_user_profile.mistral_api_key;
            response["openai_api_key"] = get_user_profile.openai_api_key;
            response["openai_organization_id"] = get_user_profile.openai_organization_id;
            response["perplexity_api_key"] = get_user_profile.perplexity_api_key;

            return crow::response(200, response.dump());

        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error" + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "Exception during update profile: " << e.what();
            return crow::response(400, "Exception during update profile" + std::string(e.what()));

        }

    }
}

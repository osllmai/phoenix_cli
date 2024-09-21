#include "api/include/controllers/prompt_controller.h"
#include "api/include/models/prompt.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_prompt(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;
                if (user_id.empty()) {
                    return crow::response(400, "User ID is required");
                }

                UserPrompt user_prompt;
                user_prompt.user_id = user_id;
                user_prompt.folder_id = request_body.value("folder_id", 0);
                user_prompt.content = request_body.value("content", "");
                user_prompt.name = request_body.value("name", "");
                user_prompt.sharing = request_body.value("sharing", "private");
                user_prompt.created_at = get_current_time();
                user_prompt.updated_at = get_current_time();

                models::Prompt::create_prompt(user_prompt);

                json response = {
                        {"message", "Prompt created"}
                };
                return crow::response(201, response.dump());
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create prompt: " << e.what();
            return crow::response(400, "During create prompt: " + std::string(e.what()));
        }
    }

    crow::response get_prompts(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);
            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;

                if (user_id.empty()) {
                    return crow::response(400, "User ID must be provided");
                }

                std::vector<UserPrompt> user_prompts = models::Prompt::prompts(user_id);

                json prompts_json = json::array();
                for (const auto &prompt: user_prompts) {
                    json prompt_json;
                    models::Prompt::to_json(prompt_json, prompt);
                    prompts_json.push_back(prompt_json);
                }

                return crow::response(200, prompts_json.dump());
            }
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_prompts: " << e.what();
            return crow::response(500, "Error during get_prompts: " + std::string(e.what()));
        }
    }

    crow::response delete_prompt(const crow::request &req, const int &prompt_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            if (models::Prompt::delete_prompt(prompt_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete prompt: " << e.what();
            return crow::response(500, "Error during delete prompt: " + std::string(e.what()));
        }
    }

    crow::response update_prompt(const crow::request &req, const int &prompt_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            UserPrompt user_prompt = models::Prompt::get_prompt_by_id(prompt_id);

            if (user_prompt.user_id.empty()) {
                return crow::response(404, "Prompt not found");
            }

            user_prompt.folder_id = request_body.value("folder_id", user_prompt.folder_id);
            user_prompt.content = request_body.value("content", user_prompt.content);
            user_prompt.name = request_body.value("name", user_prompt.name);
            user_prompt.sharing = request_body.value("sharing", user_prompt.sharing);
            user_prompt.updated_at = get_current_time();

            if (models::Prompt::update_prompt(prompt_id, user_prompt)) {
                return crow::response(200, "prompt updated");
            } else {
                return crow::response(400, "prompt not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update prompt: " << e.what();
            return crow::response(500, "Error during update prompt: " + std::string(e.what()));
        }
    }

    crow::response get_prompt_by_id(const crow::request &req, const int &prompt_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        UserPrompt user_prompt = models::Prompt::get_prompt_by_id(prompt_id);

        if (user_prompt.user_id.empty()) {
            return crow::response(404, "prompt not found");
        }

        json response;
        models::Prompt::to_json(response, user_prompt);

        return crow::response(200, response.dump());
    }

    crow::response get_prompts_by_workspace_id(const crow::request &req, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;

                if (user_id.empty()) {
                    return crow::response(400, "User ID must be provided");
                }

                std::vector<UserPrompt> user_prompts = models::Prompt::get_prompts_by_workspace_id(workspace_id);

                json prompts_json = json::array();
                for (const auto &prompt: user_prompts) {
                    json prompt_json;
                    models::Prompt::to_json(prompt_json, prompt);
                    prompts_json.push_back(prompt_json);
                }

                return crow::response(200, prompts_json.dump());
            }
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_prompts: " << e.what();
            return crow::response(500, "Error during get_prompts: " + std::string(e.what()));
        }
    }
}


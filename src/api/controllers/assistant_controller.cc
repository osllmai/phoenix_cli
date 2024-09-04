#include "api/include/controllers/assistant_controller.h"
#include "api/include/models/assistant.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {

    crow::response create_assistant(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserAssistant user_assistant;
            user_assistant.user_id = user_id;
            user_assistant.folder_id = request_body.value("folder_id", 0);
            user_assistant.name = request_body.value("name", "assistant");
            user_assistant.context_length = request_body.value("context_length", 2048);
            user_assistant.model = request_body.value("model", "gpt-4o");
            user_assistant.sharing = request_body.value("sharing", "private");
            user_assistant.prompt = request_body.value("prompt", "You are a helpful assistant");
            user_assistant.temperature = request_body.value("temperature", 0.3);
            user_assistant.description = request_body.value("description", "Default assistant");
            user_assistant.embeddings_provider = request_body.value("embeddings_provider", "nomic");
            user_assistant.include_profile_context = request_body.value("include_profile_context", false);
            user_assistant.include_workspace_instructions = request_body.value("include_workspace_instructions", false);
            user_assistant.image_path = request_body.value("image_path", "");

            models::Assistant::create_assistant(user_assistant);

            json response = {
                    {"message", "Assistant created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create assistant: " << e.what();
            return crow::response(400, "During create assistant: " + std::string(e.what()));
        }
    }

    crow::response get_assistants(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserAssistant> user_assistants = models::Assistant::assistants(user_id);

            json assistants_json = json::array();
            for (const auto &assistant: user_assistants) {
                json assistant_json;
                models::Assistant::to_json(assistant_json, assistant);
                assistants_json.push_back(assistant_json);
            }

            return crow::response(200, assistants_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_assistants: " << e.what();
            return crow::response(500, "Error during get_assistants: " + std::string(e.what()));
        }
    }

    crow::response delete_assistant(const crow::request &req, const int &assistant_id) {
        CROW_LOG_WARNING << "assistant id is " << assistant_id;
        try {
            if (models::Assistant::delete_assistant(assistant_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete assistant: " << e.what();
            return crow::response(500, "Error during delete assistant: " + std::string(e.what()));
        }
    }

    crow::response update_assistant(const crow::request &req, const int &assistant_id) {
        try {
            json request_body = json::parse(req.body);

            UserAssistant user_assistant = models::Assistant::get_assistant_by_id(assistant_id);

            if (user_assistant.user_id.empty()) {
                return crow::response(404, "Assistant not found");
            }

            user_assistant.name = request_body.value("name", user_assistant.name);
            user_assistant.context_length = request_body.value("context_length", user_assistant.context_length);
            user_assistant.model = request_body.value("model", user_assistant.model);
            user_assistant.sharing = request_body.value("sharing", user_assistant.sharing);
            user_assistant.prompt = request_body.value("prompt", user_assistant.prompt);
            user_assistant.temperature = request_body.value("temperature", user_assistant.temperature);
            user_assistant.description = request_body.value("description", user_assistant.description);
            user_assistant.embeddings_provider = request_body.value("embeddings_provider", user_assistant.embeddings_provider);
            user_assistant.include_profile_context = request_body.value("include_profile_context", user_assistant.include_profile_context);
            user_assistant.include_workspace_instructions = request_body.value("include_workspace_instructions", user_assistant.include_workspace_instructions);
            user_assistant.image_path = request_body.value("image_path", user_assistant.image_path);
            user_assistant.updated_at = get_current_time();

            if (models::Assistant::update_assistant(assistant_id, user_assistant)) {
                return crow::response(200, "assistant updated");
            } else {
                return crow::response(400, "assistant not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update assistant: " << e.what();
            return crow::response(500, "Error during update assistant: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_by_id(const crow::request &req, const int &assistant_id) {
        UserAssistant user_assistant = models::Assistant::get_assistant_by_id(assistant_id);

        if (user_assistant.user_id.empty()) {
            return crow::response(404, "assistant not found");
        }

        json response;
        models::Assistant::to_json(response, user_assistant);

        return crow::response(200, response.dump());
    }
}

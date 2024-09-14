#include "api/include/controllers/chat_controller.h"
#include "api/include/models/chat.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_chat(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserChat user_chat;
            user_chat.user_id = user_id;
            user_chat.workspace_id = request_body.value("workspace_id", 0);
            user_chat.assistant_id = request_body.value("assistant_id", 0);
            user_chat.folder_id = request_body.value("folder_id", 0);
            user_chat.name = request_body.value("name", "chat");
            user_chat.context_length = request_body.value("context_length", 2048);
            user_chat.model = request_body.value("model", "gpt-4o");
            user_chat.sharing = request_body.value("sharing", "private");
            user_chat.prompt = request_body.value("prompt", "You are a helpful assistant");
            user_chat.temperature = request_body.value("temperature", 0.3);
            user_chat.description = request_body.value("description", "Default chat");
            user_chat.embeddings_provider = request_body.value("embeddings_provider", "nomic");
            user_chat.include_profile_context = request_body.value("include_profile_context", false);
            user_chat.include_workspace_instructions = request_body.value("include_workspace_instructions", false);
            user_chat.created_at = get_current_time();
            user_chat.updated_at = get_current_time();

            models::Chat::create_chat(user_chat);

            json response = {
                    {"message", "Chat created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create chat: " << e.what();
            return crow::response(400, "During create chat: " + std::string(e.what()));
        }
    }

    crow::response get_chats(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserChat> user_chats = models::Chat::chats(user_id);

            json chats_json = json::array();
            for (const auto &chat: user_chats) {
                json chat_json;
                models::Chat::to_json(chat_json, chat);
                chats_json.push_back(chat_json);
            }

            return crow::response(200, chats_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_chats: " << e.what();
            return crow::response(500, "Error during get_chats: " + std::string(e.what()));
        }
    }

    crow::response delete_chat(const crow::request &req, const int &chat_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::Chat::delete_chat(chat_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete chat: " << e.what();
            return crow::response(500, "Error during delete chat: " + std::string(e.what()));
        }
    }

    crow::response update_chat(const crow::request &req, const int &chat_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserChat user_chat = models::Chat::get_chat_by_id(chat_id);

            if (user_chat.user_id.empty()) {
                return crow::response(404, "Chat not found");
            }

            user_chat.name = request_body.value("name", user_chat.name);
            user_chat.context_length = request_body.value("context_length", user_chat.context_length);
            user_chat.model = request_body.value("model", user_chat.model);
            user_chat.sharing = request_body.value("sharing", user_chat.sharing);
            user_chat.prompt = request_body.value("prompt", user_chat.prompt);
            user_chat.temperature = request_body.value("temperature", user_chat.temperature);
            user_chat.description = request_body.value("description", user_chat.description);
            user_chat.embeddings_provider = request_body.value("embeddings_provider", user_chat.embeddings_provider);
            user_chat.include_profile_context = request_body.value("include_profile_context",
                                                                   user_chat.include_profile_context);
            user_chat.include_workspace_instructions = request_body.value("include_workspace_instructions",
                                                                          user_chat.include_workspace_instructions);
            user_chat.updated_at = get_current_time();

            if (models::Chat::update_chat(chat_id, user_chat)) {
                return crow::response(200, "chat updated");
            } else {
                return crow::response(400, "chat not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update chat: " << e.what();
            return crow::response(500, "Error during update chat: " + std::string(e.what()));
        }
    }

    crow::response get_chat_by_id(const crow::request &req, const int &chat_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserChat user_chat = models::Chat::get_chat_by_id(chat_id);

        if (user_chat.user_id.empty()) {
            return crow::response(404, "chat not found");
        }

        json response;
        models::Chat::to_json(response, user_chat);

        return crow::response(200, response.dump());
    }

    crow::response get_chats_by_workspace_id(const crow::request &req, const int &workspace_id) {
        try {
            auto auth_head = req.get_header_value("Authorization");
            if (auth_head.empty()) {
                return crow::response(401, "No Authorization header provided");
            }

            std::vector<UserChat> user_chats = models::Chat::get_chats_by_workspace_id(workspace_id);

            json chats_json = json::array();
            for (const auto &chat: user_chats) {
                json chat_json;
                models::Chat::to_json(chat_json, chat);
                chats_json.push_back(chat_json);
            }

            return crow::response(200, chats_json.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_chats: " << e.what();
            return crow::response(500, "Error during get_chats: " + std::string(e.what()));
        }

    }
}

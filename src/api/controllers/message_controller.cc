#include "api/include/controllers/message_controller.h"
#include "api/include/models/message.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_message(const crow::request &req) {
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

            UserMessage user_message;
            user_message.user_id = user_id;
            user_message.chat_id = request_body.value("chat_id", 0);
            user_message.content = request_body.value("content", "");
            user_message.image_paths = request_body.value("image_paths", std::vector<std::string>());
            user_message.model = request_body.value("model", "");
            user_message.role = request_body.value("role", "");
            user_message.sequence_number = request_body.value("sequence_number", 0);
            user_message.created_at = get_current_time();
            user_message.updated_at = get_current_time();

            models::Message::create_message(user_message);

            json response = {
                    {"message", "Message created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create message: " << e.what();
            return crow::response(400, "During create message: " + std::string(e.what()));
        }
    }

    crow::response get_messages(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserMessage> user_messages = models::Message::messages(user_id);

            json messages_json = json::array();
            for (const auto &message : user_messages) {
                json message_json = models::Message::to_json(message);
                messages_json.push_back(message_json);
            }

            return crow::response(200, messages_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_messages: " << e.what();
            return crow::response(500, "Error during get_messages: " + std::string(e.what()));
        }
    }

    crow::response delete_message(const crow::request &req, const int &message_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::Message::delete_message(message_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete message: " << e.what();
            return crow::response(500, "Error during delete message: " + std::string(e.what()));
        }
    }

    crow::response update_message(const crow::request &req, const int &message_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserMessage user_message = models::Message::get_message_by_id(message_id);

            if (user_message.user_id.empty()) {
                return crow::response(404, "Message not found");
            }

            user_message.chat_id = request_body.value("chat_id", user_message.chat_id);
            user_message.content = request_body.value("content", user_message.content);
            user_message.image_paths = request_body.value("image_paths", user_message.image_paths);
            user_message.model = request_body.value("model", user_message.model);
            user_message.role = request_body.value("role", user_message.role);
            user_message.sequence_number = request_body.value("sequence_number", user_message.sequence_number);
            user_message.updated_at = get_current_time();

            if (models::Message::update_message(message_id, user_message)) {
                return crow::response(200, "message updated");
            } else {
                return crow::response(400, "message not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update message: " << e.what();
            return crow::response(500, "Error during update message: " + std::string(e.what()));
        }
    }

    crow::response get_message_by_id(const crow::request &req, const int &message_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserMessage user_message = models::Message::get_message_by_id(message_id);

            if (user_message.user_id.empty()) {
                return crow::response(404, "message not found");
            }

            json response = models::Message::to_json(user_message);

            return crow::response(200, response.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update message: " << e.what();
            return crow::response(500, "Error during update message: " + std::string(e.what()));
        }
    }

    crow::response get_messages_by_chat_id(const crow::request &req, const int &chat_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserMessage> user_messages = models::Message::get_messages_by_chat_id(chat_id);

            json messages_json = json::array();
            for (const auto &message: user_messages) {
                messages_json.push_back(models::Message::to_json(message));
            }

            return crow::response(200, messages_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_messages: " << e.what();
            return crow::response(500, "Error during get_messages: " + std::string(e.what()));
        }
    }
}

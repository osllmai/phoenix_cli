#include "api/include/controllers/message_file_item_controller.h"
#include "api/include/models/message_file_item.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_message_file_item(const crow::request &req) {
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

            UserMessageFileItem user_message_file_item;
            user_message_file_item.user_id = user_id;
            user_message_file_item.message_id = request_body.value("message_id", 0);
            user_message_file_item.file_item_id = request_body.value("file_item_id", 0);
            user_message_file_item.created_at = get_current_time();
            user_message_file_item.updated_at = get_current_time();

            models::MessageFileItem::create_message_file_item(user_message_file_item);

            json response = {
                    {"message", "Message File Item created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create message file item: " << e.what();
            return crow::response(400, "During create message file item: " + std::string(e.what()));
        }
    }

    crow::response get_message_file_items(const crow::request &req) {
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

            std::vector<UserMessageFileItem> user_message_file_items = models::MessageFileItem::message_file_items(
                    user_id);

            json message_file_items_json = json::array();
            for (const auto &message_file_item: user_message_file_items) {
                json message_file_item_json;
                models::MessageFileItem::to_json(message_file_item_json, message_file_item);
                message_file_items_json.push_back(message_file_item_json);
            }

            return crow::response(200, message_file_items_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_message_file_items: " << e.what();
            return crow::response(500, "Error during get_message_file_items: " + std::string(e.what()));
        }
    }

    crow::response delete_message_file_item(const crow::request &req, const int &message_id, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::MessageFileItem::delete_message_file_item(message_id, file_item_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete message file item: " << e.what();
            return crow::response(500, "Error during delete message file item: " + std::string(e.what()));
        }
    }

    crow::response update_message_file_item(const crow::request &req, const int &message_id, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserMessageFileItem user_message_file_item = models::MessageFileItem::get_message_file_item_by_id(
                    message_id, file_item_id);

            if (user_message_file_item.user_id.empty()) {
                return crow::response(404, "Message File Item not found");
            }

            user_message_file_item.user_id = request_body.value("user_id", user_message_file_item.user_id);
            user_message_file_item.updated_at = get_current_time();

            if (models::MessageFileItem::update_message_file_item(message_id, file_item_id, user_message_file_item)) {
                return crow::response(200, "message file item updated");
            } else {
                return crow::response(400, "message file item not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update message file item: " << e.what();
            return crow::response(500, "Error during update message file item: " + std::string(e.what()));
        }
    }

    crow::response
    get_message_file_item_by_id(const crow::request &req, const int &message_id, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserMessageFileItem user_message_file_item = models::MessageFileItem::get_message_file_item_by_id(message_id,
                                                                                                          file_item_id);

        if (user_message_file_item.user_id.empty()) {
            return crow::response(404, "message file item not found");
        }

        json response;
        models::MessageFileItem::to_json(response, user_message_file_item);

        return crow::response(200, response.dump());
    }
}

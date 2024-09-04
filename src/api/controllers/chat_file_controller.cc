#include "api/include/controllers/chat_file_controller.h"
#include "api/include/models/chat_file.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_chat_file(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserChatFile user_chat_file;
            user_chat_file.user_id = user_id;
            user_chat_file.chat_id = request_body.value("chat_id", 0);
            user_chat_file.file_id = request_body.value("file_id", 0);

            models::ChatFile::create_chat_file(user_chat_file);

            json response = {
                    {"message", "Chat File created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create chat file: " << e.what();
            return crow::response(400, "During create chat file: " + std::string(e.what()));
        }
    }

    crow::response get_chat_files(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserChatFile> user_chat_files = models::ChatFile::chat_files(user_id);

            json chat_files_json = json::array();
            for (const auto &chat_file: user_chat_files) {
                json chat_file_json;
                models::ChatFile::to_json(chat_file_json, chat_file);
                chat_files_json.push_back(chat_file_json);
            }

            return crow::response(200, chat_files_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_chat_files: " << e.what();
            return crow::response(500, "Error during get_chat_files: " + std::string(e.what()));
        }
    }

    crow::response delete_chat_file(const crow::request &req, const int &chat_id, const int &file_id) {
        try {
            if (models::ChatFile::delete_chat_file(chat_id, file_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete chat file: " << e.what();
            return crow::response(500, "Error during delete chat file: " + std::string(e.what()));
        }
    }

    crow::response update_chat_file(const crow::request &req, const int &chat_id, const int &file_id) {
        try {
            json request_body = json::parse(req.body);

            UserChatFile user_chat_file = models::ChatFile::get_chat_file_by_id(chat_id, file_id);

            if (user_chat_file.user_id.empty()) {
                return crow::response(404, "Chat File not found");
            }

            user_chat_file.user_id = request_body.value("user_id", user_chat_file.user_id);
            user_chat_file.updated_at = get_current_time();

            if (models::ChatFile::update_chat_file(chat_id, file_id, user_chat_file)) {
                return crow::response(200, "chat file updated");
            } else {
                return crow::response(400, "chat file not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update chat file: " << e.what();
            return crow::response(500, "Error during update chat file: " + std::string(e.what()));
        }
    }

    crow::response get_chat_file_by_id(const crow::request &req, const int &chat_id, const int &file_id) {
        UserChatFile user_chat_file = models::ChatFile::get_chat_file_by_id(chat_id, file_id);

        if (user_chat_file.user_id.empty()) {
            return crow::response(404, "chat file not found");
        }

        json response;
        models::ChatFile::to_json(response, user_chat_file);

        return crow::response(200, response.dump());
    }
}

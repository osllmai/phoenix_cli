#include "api/include/controllers/assistant_file_controller.h"
#include "api/include/models/assistant_file.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_assistant_file(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserAssistantFile assistant_file;
            assistant_file.user_id = user_id;
            assistant_file.assistant_id = request_body.value("assistant_id", 0);
            assistant_file.file_id = request_body.value("file_id", 0);
            assistant_file.created_at = get_current_time();
            assistant_file.updated_at = get_current_time();

            models::AssistantFile::create_assistant_file(assistant_file);

            json response = {
                    {"message", "Assistant file created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create assistant file: " << e.what();
            return crow::response(400, "During create assistant file: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_files(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserAssistantFile> assistant_files = models::AssistantFile::assistant_files(user_id);

            json assistant_files_json = json::array();
            for (const auto &assistant_file: assistant_files) {
                json assistant_file_json;
                models::AssistantFile::to_json(assistant_file_json, assistant_file);
                assistant_files_json.push_back(assistant_file_json);
            }

            return crow::response(200, assistant_files_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_assistant_files: " << e.what();
            return crow::response(500, "Error during get_assistant_files: " + std::string(e.what()));
        }
    }

    crow::response delete_assistant_file(const crow::request &req, const int &assistant_id, const int &file_id) {
        try {
            if (models::AssistantFile::delete_assistant_file(assistant_id, file_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete assistant file: " << e.what();
            return crow::response(500, "Error during delete assistant file: " + std::string(e.what()));
        }
    }

    crow::response update_assistant_file(const crow::request &req, const int &assistant_id, const int &file_id) {
        try {
            json request_body = json::parse(req.body);

            UserAssistantFile assistant_file = models::AssistantFile::get_assistant_file_by_id(assistant_id, file_id);

            if (assistant_file.user_id.empty()) {
                return crow::response(404, "Assistant file not found");
            }

            assistant_file.user_id = request_body.value("user_id", assistant_file.user_id);
            assistant_file.updated_at = get_current_time();

            if (models::AssistantFile::update_assistant_file(assistant_id, file_id, assistant_file)) {
                return crow::response(200, "assistant file updated");
            } else {
                return crow::response(400, "assistant file not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update assistant file: " << e.what();
            return crow::response(500, "Error during update assistant file: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_file_by_id(const crow::request &req, const int &assistant_id, const int &file_id) {
        UserAssistantFile assistant_file = models::AssistantFile::get_assistant_file_by_id(assistant_id, file_id);

        if (assistant_file.user_id.empty()) {
            return crow::response(404, "assistant file not found");
        }

        json response;
        models::AssistantFile::to_json(response, assistant_file);

        return crow::response(200, response.dump());
    }
}

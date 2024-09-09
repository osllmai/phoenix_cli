#include "api/include/controllers/assistant_collection_controller.h"
#include "api/include/models/assistant_collection.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_assistant_collection(const crow::request &req) {
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

            UserAssistantCollection assistant_collection;
            assistant_collection.user_id = user_id;
            assistant_collection.assistant_id = request_body.value("assistant_id", 0);
            assistant_collection.collection_id = request_body.value("collection_id", 0);
            assistant_collection.created_at = get_current_time();
            assistant_collection.updated_at = get_current_time();

            models::AssistantCollection::create_assistant_collection(assistant_collection);

            json response = {
                    {"message", "Assistant collection created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create assistant collection: " << e.what();
            return crow::response(400, "During create assistant collection: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_collections(const crow::request &req) {
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

            std::vector<UserAssistantCollection> assistant_collections = models::AssistantCollection::assistant_collections(user_id);

            json assistant_collections_json = json::array();
            for (const auto &assistant_collection: assistant_collections) {
                json assistant_collection_json;
                models::AssistantCollection::to_json(assistant_collection_json, assistant_collection);
                assistant_collections_json.push_back(assistant_collection_json);
            }

            return crow::response(200, assistant_collections_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_assistant_collections: " << e.what();
            return crow::response(500, "Error during get_assistant_collections: " + std::string(e.what()));
        }
    }

    crow::response delete_assistant_collection(const crow::request &req, const int &assistant_id, const int &collection_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::AssistantCollection::delete_assistant_collection(assistant_id, collection_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete assistant collection: " << e.what();
            return crow::response(500, "Error during delete assistant collection: " + std::string(e.what()));
        }
    }

    crow::response update_assistant_collection(const crow::request &req, const int &assistant_id, const int &collection_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserAssistantCollection assistant_collection = models::AssistantCollection::get_assistant_collection_by_id(assistant_id, collection_id);

            if (assistant_collection.user_id.empty()) {
                return crow::response(404, "Assistant collection not found");
            }

            auto get_user_id = get_user_id_from_token(auth_header);
            assistant_collection.user_id = *get_user_id;
            assistant_collection.updated_at = get_current_time();

            if (models::AssistantCollection::update_assistant_collection(assistant_id, collection_id, assistant_collection)) {
                return crow::response(200, "assistant collection updated");
            } else {
                return crow::response(400, "assistant collection not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update assistant collection: " << e.what();
            return crow::response(500, "Error during update assistant collection: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_collection_by_id(const crow::request &req, const int &assistant_id, const int &collection_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserAssistantCollection assistant_collection = models::AssistantCollection::get_assistant_collection_by_id(assistant_id, collection_id);

        if (assistant_collection.user_id.empty()) {
            return crow::response(404, "assistant collection not found");
        }

        json response;
        models::AssistantCollection::to_json(response, assistant_collection);

        return crow::response(200, response.dump());
    }
}

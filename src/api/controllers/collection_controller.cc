#include "api/include/controllers/collection_controller.h"
#include "api/include/models/collection.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_collection(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserCollection collection;
            collection.user_id = user_id;
            collection.folder_id = request_body.value("folder_id", 0);
            collection.name = request_body.value("name", "collection");
            collection.sharing = request_body.value("sharing", "private");
            collection.description = request_body.value("description", "Default collection");
            collection.created_at = get_current_time();
            collection.updated_at = get_current_time();

            models::Collection::create_collection(collection);

            json response = {
                    {"message", "Collection created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create collection: " << e.what();
            return crow::response(400, "During create collection: " + std::string(e.what()));
        }
    }

    crow::response get_collections(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserCollection> collections = models::Collection::collections(user_id);

            json collections_json = json::array();
            for (const auto &collection: collections) {
                json collection_json;
                models::Collection::to_json(collection_json, collection);
                collections_json.push_back(collection_json);
            }

            return crow::response(200, collections_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_collections: " << e.what();
            return crow::response(500, "Error during get_collections: " + std::string(e.what()));
        }
    }

    crow::response delete_collection(const crow::request &req, const int &collection_id) {
        try {
            if (models::Collection::delete_collection(collection_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete collection: " << e.what();
            return crow::response(500, "Error during delete collection: " + std::string(e.what()));
        }
    }

    crow::response update_collection(const crow::request &req, const int &collection_id) {
        try {
            json request_body = json::parse(req.body);

            UserCollection collection = models::Collection::get_collection_by_id(collection_id);

            if (collection.user_id.empty()) {
                return crow::response(404, "Collection not found");
            }

            collection.name = request_body.value("name", collection.name);
            collection.sharing = request_body.value("sharing", collection.sharing);
            collection.description = request_body.value("description", collection.description);
            collection.folder_id = request_body.value("folder_id", collection.folder_id);
            collection.updated_at = get_current_time();

            if (models::Collection::update_collection(collection_id, collection)) {
                return crow::response(200, "collection updated");
            } else {
                return crow::response(400, "collection not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update collection: " << e.what();
            return crow::response(500, "Error during update collection: " + std::string(e.what()));
        }
    }

    crow::response get_collection_by_id(const crow::request &req, const int &collection_id) {
        UserCollection collection = models::Collection::get_collection_by_id(collection_id);

        if (collection.user_id.empty()) {
            return crow::response(404, "collection not found");
        }

        json response;
        models::Collection::to_json(response, collection);

        return crow::response(200, response.dump());
    }
}

#include "api/include/controllers/collection_file_controller.h"
#include "api/include/models/collection_file.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_collection_file(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserCollectionFile collection_file;
            collection_file.user_id = user_id;
            collection_file.collection_id = request_body.value("collection_id", 0);
            collection_file.file_id = request_body.value("file_id", 0);
            collection_file.created_at = get_current_time();
            collection_file.updated_at = get_current_time();

            models::CollectionFile::create_collection_file(collection_file);

            json response = {
                    {"message", "Collection file created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create collection file: " << e.what();
            return crow::response(400, "During create collection file: " + std::string(e.what()));
        }
    }

    crow::response get_collection_files(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserCollectionFile> collection_files = models::CollectionFile::collection_files(user_id);

            json collection_files_json = json::array();
            for (const auto &collection_file: collection_files) {
                json collection_file_json;
                models::CollectionFile::to_json(collection_file_json, collection_file);
                collection_files_json.push_back(collection_file_json);
            }

            return crow::response(200, collection_files_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_collection_files: " << e.what();
            return crow::response(500, "Error during get_collection_files: " + std::string(e.what()));
        }
    }

    crow::response delete_collection_file(const crow::request &req, const int &collection_id, const int &file_id) {
        try {
            if (models::CollectionFile::delete_collection_file(collection_id, file_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete collection file: " << e.what();
            return crow::response(500, "Error during delete collection file: " + std::string(e.what()));
        }
    }

    crow::response update_collection_file(const crow::request &req, const int &collection_id, const int &file_id) {
        try {
            json request_body = json::parse(req.body);

            UserCollectionFile collection_file = models::CollectionFile::get_collection_file_by_id(collection_id,
                                                                                                   file_id);

            if (collection_file.user_id.empty()) {
                return crow::response(404, "Collection file not found");
            }

            collection_file.user_id = request_body.value("user_id", collection_file.user_id);
            collection_file.updated_at = get_current_time();

            if (models::CollectionFile::update_collection_file(collection_id, file_id, collection_file)) {
                return crow::response(200, "collection file updated");
            } else {
                return crow::response(400, "collection file not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update collection file: " << e.what();
            return crow::response(500, "Error during update collection file: " + std::string(e.what()));
        }
    }

    crow::response get_collection_file_by_id(const crow::request &req, const int &collection_id, const int &file_id) {
        UserCollectionFile collection_file = models::CollectionFile::get_collection_file_by_id(collection_id, file_id);

        if (collection_file.user_id.empty()) {
            return crow::response(404, "collection file not found");
        }

        json response;
        models::CollectionFile::to_json(response, collection_file);

        return crow::response(200, response.dump());
    }
}


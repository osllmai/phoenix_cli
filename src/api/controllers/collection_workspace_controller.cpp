#include "api/include/controllers/collection_workspace_controller.h"
#include "api/include/models/collection_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_collection_workspace(const crow::request &req) {
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

            UserCollectionWorkspace collection_workspace;
            collection_workspace.user_id = user_id;
            collection_workspace.collection_id = request_body.value("collection_id", 0);
            collection_workspace.workspace_id = request_body.value("workspace_id", 0);
            collection_workspace.created_at = get_current_time();
            collection_workspace.updated_at = get_current_time();

            models::CollectionWorkspace::create_collection_workspace(collection_workspace);

            json response = {
                    {"message", "Collection workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create collection workspace: " << e.what();
            return crow::response(400, "During create collection workspace: " + std::string(e.what()));
        }
    }

    crow::response get_collection_workspaces(const crow::request &req) {
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

            std::vector<UserCollectionWorkspace> collection_workspaces = models::CollectionWorkspace::collection_workspaces(
                    user_id);

            json collection_workspaces_json = json::array();
            for (const auto &collection_workspace: collection_workspaces) {
                json collection_workspace_json;
                models::CollectionWorkspace::to_json(collection_workspace_json, collection_workspace);
                collection_workspaces_json.push_back(collection_workspace_json);
            }

            return crow::response(200, collection_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_collection_workspaces: " << e.what();
            return crow::response(500, "Error during get_collection_workspaces: " + std::string(e.what()));
        }
    }

    crow::response
    delete_collection_workspace(const crow::request &req, const int &collection_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::CollectionWorkspace::delete_collection_workspace(collection_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete collection workspace: " << e.what();
            return crow::response(500, "Error during delete collection workspace: " + std::string(e.what()));
        }
    }

    crow::response
    update_collection_workspace(const crow::request &req, const int &collection_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserCollectionWorkspace collection_workspace = models::CollectionWorkspace::get_collection_workspace_by_id(
                    collection_id, workspace_id);

            if (collection_workspace.user_id.empty()) {
                return crow::response(404, "Collection workspace not found");
            }
            auto get_user_id = get_user_id_from_token(auth_header);
            collection_workspace.user_id = *get_user_id;
            collection_workspace.updated_at = get_current_time();

            if (models::CollectionWorkspace::update_collection_workspace(collection_id, workspace_id,
                                                                         collection_workspace)) {
                return crow::response(200, "collection workspace updated");
            } else {
                return crow::response(400, "collection workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update collection workspace: " << e.what();
            return crow::response(500, "Error during update collection workspace: " + std::string(e.what()));
        }
    }

    crow::response
    get_collection_workspace_by_id(const crow::request &req, const int &collection_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserCollectionWorkspace collection_workspace = models::CollectionWorkspace::get_collection_workspace_by_id(
                collection_id, workspace_id);

        if (collection_workspace.user_id.empty()) {
            return crow::response(404, "collection workspace not found");
        }

        json response;
        models::CollectionWorkspace::to_json(response, collection_workspace);

        return crow::response(200, response.dump());
    }

}

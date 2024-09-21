#include "api/include/controllers/model_workspace_controller.h"
#include "api/include/models/model_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_model_workspace(const crow::request &req) {
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

            UserModelWorkspace model_workspace;
            model_workspace.user_id = user_id;
            model_workspace.model_id = request_body.value("model_id", 0);
            model_workspace.workspace_id = request_body.value("workspace_id", 0);
            model_workspace.created_at = get_current_time();
            model_workspace.updated_at = get_current_time();

            models::ModelWorkspace::create_model_workspace(model_workspace);

            json response = {
                    {"message", "Model workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create model workspace: " << e.what();
            return crow::response(400, "During create model workspace: " + std::string(e.what()));
        }
    }

    crow::response get_model_workspaces(const crow::request &req) {
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

            std::vector<UserModelWorkspace> model_workspaces = models::ModelWorkspace::model_workspaces(user_id);

            json model_workspaces_json = json::array();
            for (const auto &model_workspace: model_workspaces) {
                json model_workspace_json;
                models::ModelWorkspace::to_json(model_workspace_json, model_workspace);
                model_workspaces_json.push_back(model_workspace_json);
            }

            return crow::response(200, model_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_model_workspaces: " << e.what();
            return crow::response(500, "Error during get_model_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_model_workspace(const crow::request &req, const int &model_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::ModelWorkspace::delete_model_workspace(model_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete model workspace: " << e.what();
            return crow::response(500, "Error during delete model workspace: " + std::string(e.what()));
        }
    }

    crow::response update_model_workspace(const crow::request &req, const int &model_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserModelWorkspace model_workspace = models::ModelWorkspace::get_model_workspace_by_id(model_id, workspace_id);

            if (model_workspace.user_id.empty()) {
                return crow::response(404, "Model workspace not found");
            }

            model_workspace.user_id = request_body.value("user_id", model_workspace.user_id);
            model_workspace.updated_at = get_current_time();

            if (models::ModelWorkspace::update_model_workspace(model_id, workspace_id, model_workspace)) {
                return crow::response(200, "model workspace updated");
            } else {
                return crow::response(400, "model workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update model workspace: " << e.what();
            return crow::response(500, "Error during update model workspace: " + std::string(e.what()));
        }
    }

    crow::response get_model_workspace_by_id(const crow::request &req, const int &model_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserModelWorkspace model_workspace = models::ModelWorkspace::get_model_workspace_by_id(model_id, workspace_id);

        if (model_workspace.user_id.empty()) {
            return crow::response(404, "model workspace not found");
        }

        json response;
        models::ModelWorkspace::to_json(response, model_workspace);

        return crow::response(200, response.dump());
    }
}

#include "api/include/controllers/preset_workspace_controller.h"
#include "api/include/models/preset_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;


namespace controllers {

    crow::response create_preset_workspace(const crow::request &req) {
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

            UserPresetWorkspace user_preset_workspace;
            user_preset_workspace.user_id = user_id;
            user_preset_workspace.preset_id = request_body.value("preset_id", 0);
            user_preset_workspace.workspace_id = request_body.value("workspace_id", 0);

            models::PresetWorkspace::create_preset_workspace(user_preset_workspace);

            json response = {
                    {"message", "Preset Workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create preset workspace: " << e.what();
            return crow::response(400, "During create preset workspace: " + std::string(e.what()));
        }
    }

    crow::response get_preset_workspaces(const crow::request &req) {
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

            std::vector<UserPresetWorkspace> user_preset_workspaces = models::PresetWorkspace::preset_workspaces(
                    user_id);

            json preset_workspaces_json = json::array();
            for (const auto &preset_workspace: user_preset_workspaces) {
                json preset_workspace_json;
                models::PresetWorkspace::to_json(preset_workspace_json, preset_workspace);
                preset_workspaces_json.push_back(preset_workspace_json);
            }

            return crow::response(200, preset_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_preset_workspaces: " << e.what();
            return crow::response(500, "Error during get_preset_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_preset_workspace(const crow::request &req, const int &preset_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::PresetWorkspace::delete_preset_workspace(preset_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete preset workspace: " << e.what();
            return crow::response(500, "Error during delete preset workspace: " + std::string(e.what()));
        }
    }

    crow::response update_preset_workspace(const crow::request &req, const int &preset_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserPresetWorkspace user_preset_workspace = models::PresetWorkspace::get_preset_workspace_by_id(preset_id,
                                                                                                            workspace_id);

            if (user_preset_workspace.user_id.empty()) {
                return crow::response(404, "Preset Workspace not found");
            }

            user_preset_workspace.user_id = request_body.value("user_id", user_preset_workspace.user_id);
            user_preset_workspace.updated_at = get_current_time();

            if (models::PresetWorkspace::update_preset_workspace(preset_id, workspace_id, user_preset_workspace)) {
                return crow::response(200, "preset workspace updated");
            } else {
                return crow::response(400, "preset workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update preset workspace: " << e.what();
            return crow::response(500, "Error during update preset workspace: " + std::string(e.what()));
        }
    }

    crow::response get_preset_workspace_by_id(const crow::request &req, const int &preset_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserPresetWorkspace user_preset_workspace = models::PresetWorkspace::get_preset_workspace_by_id(preset_id,
                                                                                                        workspace_id);

        if (user_preset_workspace.user_id.empty()) {
            return crow::response(404, "preset workspace not found");
        }

        json response;
        models::PresetWorkspace::to_json(response, user_preset_workspace);

        return crow::response(200, response.dump());
    }
}

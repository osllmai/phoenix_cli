#include "api/include/controllers/tool_workspace_controller.h"
#include "api/include/models/tool_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_tool_workspace(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            auto user_id_opt = get_user_id_from_token(auth_header);
            std::string user_id = *user_id_opt;

            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserToolWorkspace tool_workspace;
            tool_workspace.user_id = user_id;
            tool_workspace.tool_id = request_body.value("tool_id", 0);
            tool_workspace.workspace_id = request_body.value("workspace_id", 0);
            tool_workspace.created_at = get_current_time();
            tool_workspace.updated_at = get_current_time();

            models::ToolWorkspace::create_tool_workspace(tool_workspace);

            json response = {
                    {"message", "Tool workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create tool workspace: " << e.what();
            return crow::response(400, "During create tool workspace: " + std::string(e.what()));
        }
    }

    crow::response get_tool_workspaces(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            auto user_id_opt = get_user_id_from_token(auth_header);
            std::string user_id = *user_id_opt;

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserToolWorkspace> tool_workspaces = models::ToolWorkspace::tool_workspaces(user_id);

            json tool_workspaces_json = json::array();
            for (const auto &tool_workspace: tool_workspaces) {
                json tool_workspace_json;
                models::ToolWorkspace::to_json(tool_workspace_json, tool_workspace);
                tool_workspaces_json.push_back(tool_workspace_json);
            }

            return crow::response(200, tool_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_tool_workspaces: " << e.what();
            return crow::response(500, "Error during get_tool_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_tool_workspace(const crow::request &req, const int &tool_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            if (models::ToolWorkspace::delete_tool_workspace(tool_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete tool workspace: " << e.what();
            return crow::response(500, "Error during delete tool workspace: " + std::string(e.what()));
        }
    }

    crow::response update_tool_workspace(const crow::request &req, const int &tool_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            UserToolWorkspace tool_workspace = models::ToolWorkspace::get_tool_workspace_by_id(tool_id, workspace_id);

            if (tool_workspace.user_id.empty()) {
                return crow::response(404, "Tool workspace not found");
            }
            auto user_id_opt = get_user_id_from_token(auth_header);

            tool_workspace.user_id = *user_id_opt;
            tool_workspace.updated_at = get_current_time();

            if (models::ToolWorkspace::update_tool_workspace(tool_id, workspace_id, tool_workspace)) {
                return crow::response(200, "tool workspace updated");
            } else {
                return crow::response(400, "tool workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update tool workspace: " << e.what();
            return crow::response(500, "Error during update tool workspace: " + std::string(e.what()));
        }
    }

    crow::response get_tool_workspace_by_id(const crow::request &req, const int &tool_id, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        UserToolWorkspace tool_workspace = models::ToolWorkspace::get_tool_workspace_by_id(tool_id, workspace_id);

        if (tool_workspace.user_id.empty()) {
            return crow::response(404, "tool workspace not found");
        }

        json response;
        models::ToolWorkspace::to_json(response, tool_workspace);

        return crow::response(200, response.dump());
    }
}


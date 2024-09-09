#include "api/include/controllers/tool_workspace_controller.h"
#include "api/include/models/tool_workspace.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;

std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

namespace controllers {
    crow::response create_tool_workspace(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
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
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

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
        try {
            json request_body = json::parse(req.body);

            UserToolWorkspace tool_workspace = models::ToolWorkspace::get_tool_workspace_by_id(tool_id, workspace_id);

            if (tool_workspace.user_id.empty()) {
                return crow::response(404, "Tool workspace not found");
            }

            tool_workspace.user_id = request_body.value("user_id", tool_workspace.user_id);
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
        UserToolWorkspace tool_workspace = models::ToolWorkspace::get_tool_workspace_by_id(tool_id, workspace_id);

        if (tool_workspace.user_id.empty()) {
            return crow::response(404, "tool workspace not found");
        }

        json response;
        models::ToolWorkspace::to_json(response, tool_workspace);

        return crow::response(200, response.dump());
    }
}


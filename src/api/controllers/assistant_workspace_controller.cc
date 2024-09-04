#include "api/include/controllers/assistant_workspace_controller.h"
#include "api/include/models/assistant_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_assistant_workspace(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserAssistantWorkspace user_assistant_workspace;
            user_assistant_workspace.user_id = user_id;
            user_assistant_workspace.assistant_id = request_body.value("assistant_id", 0);
            user_assistant_workspace.workspace_id = request_body.value("workspace_id", 0);

            models::AssistantWorkspace::create_assistant_workspace(user_assistant_workspace);

            json response = {
                    {"message", "Assistant Workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create assistant workspace: " << e.what();
            return crow::response(400, "During create assistant workspace: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_workspaces(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserAssistantWorkspace> user_assistant_workspaces = models::AssistantWorkspace::assistant_workspaces(user_id);

            json assistant_workspaces_json = json::array();
            for (const auto &assistant_workspace: user_assistant_workspaces) {
                json assistant_workspace_json;
                models::AssistantWorkspace::to_json(assistant_workspace_json, assistant_workspace);
                assistant_workspaces_json.push_back(assistant_workspace_json);
            }

            return crow::response(200, assistant_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_assistant_workspaces: " << e.what();
            return crow::response(500, "Error during get_assistant_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_assistant_workspace(const crow::request &req, const int &assistant_id, const int &workspace_id) {
        try {
            if (models::AssistantWorkspace::delete_assistant_workspace(assistant_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete assistant workspace: " << e.what();
            return crow::response(500, "Error during delete assistant workspace: " + std::string(e.what()));
        }
    }

    crow::response update_assistant_workspace(const crow::request &req, const int &assistant_id, const int &workspace_id) {
        try {
            json request_body = json::parse(req.body);

            UserAssistantWorkspace user_assistant_workspace = models::AssistantWorkspace::get_assistant_workspace_by_id(assistant_id, workspace_id);

            if (user_assistant_workspace.user_id.empty()) {
                return crow::response(404, "Assistant Workspace not found");
            }

            user_assistant_workspace.user_id = request_body.value("user_id", user_assistant_workspace.user_id);
            user_assistant_workspace.updated_at = get_current_time();

            if (models::AssistantWorkspace::update_assistant_workspace(assistant_id, workspace_id, user_assistant_workspace)) {
                return crow::response(200, "assistant workspace updated");
            } else {
                return crow::response(400, "assistant workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update assistant workspace: " << e.what();
            return crow::response(500, "Error during update assistant workspace: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_workspace_by_id(const crow::request &req, const int &assistant_id, const int &workspace_id) {
        UserAssistantWorkspace user_assistant_workspace = models::AssistantWorkspace::get_assistant_workspace_by_id(assistant_id, workspace_id);

        if (user_assistant_workspace.user_id.empty()) {
            return crow::response(404, "assistant workspace not found");
        }

        json response;
        models::AssistantWorkspace::to_json(response, user_assistant_workspace);

        return crow::response(200, response.dump());
    }
}

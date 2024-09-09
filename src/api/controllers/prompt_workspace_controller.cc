#include "api/include/controllers/prompt_workspace_controller.h"
#include "api/include/models/prompt_workspace.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_prompt_workspace(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserPromptWorkspace prompt_workspace;
            prompt_workspace.user_id = user_id;
            prompt_workspace.prompt_id = request_body.value("prompt_id", 0);
            prompt_workspace.workspace_id = request_body.value("workspace_id", 0);
            prompt_workspace.created_at = get_current_time();
            prompt_workspace.updated_at = get_current_time();

            models::PromptWorkspace::create_prompt_workspace(prompt_workspace);

            json response = {
                    {"message", "Prompt workspace created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create prompt workspace: " << e.what();
            return crow::response(400, "During create prompt workspace: " + std::string(e.what()));
        }
    }

    crow::response get_prompt_workspaces(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserPromptWorkspace> prompt_workspaces = models::PromptWorkspace::prompt_workspaces(user_id);

            json prompt_workspaces_json = json::array();
            for (const auto &prompt_workspace: prompt_workspaces) {
                json prompt_workspace_json;
                models::PromptWorkspace::to_json(prompt_workspace_json, prompt_workspace);
                prompt_workspaces_json.push_back(prompt_workspace_json);
            }

            return crow::response(200, prompt_workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_prompt_workspaces: " << e.what();
            return crow::response(500, "Error during get_prompt_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_prompt_workspace(const crow::request &req, const int &prompt_id, const int &workspace_id) {
        try {
            if (models::PromptWorkspace::delete_prompt_workspace(prompt_id, workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete prompt workspace: " << e.what();
            return crow::response(500, "Error during delete prompt workspace: " + std::string(e.what()));
        }
    }

    crow::response update_prompt_workspace(const crow::request &req, const int &prompt_id, const int &workspace_id) {
        try {
            json request_body = json::parse(req.body);

            UserPromptWorkspace prompt_workspace = models::PromptWorkspace::get_prompt_workspace_by_id(prompt_id, workspace_id);

            if (prompt_workspace.user_id.empty()) {
                return crow::response(404, "Prompt workspace not found");
            }

            prompt_workspace.user_id = request_body.value("user_id", prompt_workspace.user_id);
            prompt_workspace.updated_at = get_current_time();

            if (models::PromptWorkspace::update_prompt_workspace(prompt_id, workspace_id, prompt_workspace)) {
                return crow::response(200, "prompt workspace updated");
            } else {
                return crow::response(400, "prompt workspace not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update prompt workspace: " << e.what();
            return crow::response(500, "Error during update prompt workspace: " + std::string(e.what()));
        }
    }

    crow::response get_prompt_workspace_by_id(const crow::request &req, const int &prompt_id, const int &workspace_id) {
        UserPromptWorkspace prompt_workspace = models::PromptWorkspace::get_prompt_workspace_by_id(prompt_id, workspace_id);

        if (prompt_workspace.user_id.empty()) {
            return crow::response(404, "prompt workspace not found");
        }

        json response;
        models::PromptWorkspace::to_json(response, prompt_workspace);

        return crow::response(200, response.dump());
    }
}

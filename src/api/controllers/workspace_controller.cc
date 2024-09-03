#include "api/include/controllers/workspace_controller.h"
#include "api/include/models/workspace.h"

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
    crow::response create_workspace(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID  is required");
            }

            if (request_body.value("is_home", false)) {
                models::Workspace::disable_home_workspaces();
            }

            UserWorkspace user_workspace;
            user_workspace.user_id = user_id;
            user_workspace.name = request_body.value("name", "workspace");
            user_workspace.default_context_length = request_body.value("default_context_length", 2048);
            user_workspace.default_model = request_body.value("default_model", "gpt-4o");
            user_workspace.sharing = request_body.value("sharing", "private");
            user_workspace.default_prompt = request_body.value("default_prompt", "You are a helpful assistant");
            user_workspace.default_temperature = request_body.value("default_temperature", 0.3);
            user_workspace.description = request_body.value("description", "Default workspace");
            user_workspace.embeddings_provider = request_body.value("embeddings_provider", "nomic");
            user_workspace.include_profile_context = request_body.value("include_profile_context", false);
            user_workspace.include_workspace_instructions = request_body.value("include_workspace_instructions", false);
            user_workspace.instructions = request_body.value("instructions", "no instructions");
            user_workspace.is_home = request_body.value("is_home", false);

            models::Workspace::create_workspace(user_workspace);

            json response = {
                    {"message", "Workspace created"}
            };
            return crow::response(201, response.dump());


        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create workspace: " << e.what();
            return crow::response(400, "During create workspace: " + std::string(e.what()));
        }
    }

    crow::response get_workspaces(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserWorkspace> user_workspaces = models::Workspace::workspaces(user_id);

            json workspaces_json = json::array();
            for (const auto &workspace: user_workspaces) {
                json workspace_json;
                models::Workspace::to_json(workspace_json, workspace);
                workspaces_json.push_back(workspace_json);
            }

            return crow::response(200, workspaces_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_workspaces: " << e.what();
            return crow::response(500, "Error during get_workspaces: " + std::string(e.what()));
        }
    }

    crow::response delete_workspace(const crow::request &req, const int &workspace_id) {
        try {
            if (models::Workspace::delete_workspace(workspace_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }


        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete workspace: " << e.what();
            return crow::response(500, "Error during delete workspace: " + std::string(e.what()));
        }
    }

    crow::response update_workspace(const crow::request &req, const int &workspace_id) {
        try {
            json request_body = json::parse(req.body);

            UserWorkspace user_workspace = models::Workspace::get_workspace_by_id(workspace_id);

            if (user_workspace.user_id.empty()) {
                return crow::response(404, "Workspace not found");
            }

            if (request_body.value("is_home", user_workspace.is_home)) {
                models::Workspace::disable_home_workspaces();
            }
            user_workspace.name = request_body.value("name", user_workspace.name);
            user_workspace.is_home = request_body.value("is_home", user_workspace.is_home);
            user_workspace.instructions = request_body.value("instructions", user_workspace.instructions);
            user_workspace.include_workspace_instructions = request_body.value("include_workspace_instructions",
                                                                               user_workspace.include_workspace_instructions);
            user_workspace.include_profile_context = request_body.value("include_profile_context",
                                                                        user_workspace.include_profile_context);
            user_workspace.embeddings_provider = request_body.value("embeddings_provider",
                                                                    user_workspace.embeddings_provider);
            user_workspace.description = request_body.value("description", user_workspace.description);
            user_workspace.default_temperature = request_body.value("default_temperature",
                                                                    user_workspace.default_temperature);
            user_workspace.default_prompt = request_body.value("default_prompt", user_workspace.default_prompt);
            user_workspace.sharing = request_body.value("sharing", user_workspace.sharing);
            user_workspace.default_model = request_body.value("default_model", user_workspace.default_model);
            user_workspace.default_context_length = request_body.value("default_context_length",
                                                                       user_workspace.default_context_length);
            user_workspace.updated_at = get_current_time();

            if (models::Workspace::update_workspace(workspace_id, user_workspace)) {
                return crow::response(200, "workspace updated");
            } else {
                return crow::response(400, "workspace not updated");

            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update workspace: " << e.what();
            return crow::response(500, "Error during update workspace: " + std::string(e.what()));
        }
    }

    crow::response get_workspace_by_id(const crow::request &req, const int &workspace_id ) {
        UserWorkspace user_workspace = models::Workspace::get_workspace_by_id(workspace_id);

        if (user_workspace.user_id.empty()) {
            return crow::response(404, "workspace not found");
        }

        json response;
        models::Workspace::to_json(response, user_workspace);

        return crow::response(200, response.dump());
    }
}
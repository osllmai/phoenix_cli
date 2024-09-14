#include "api/include/controllers/tool_controller.h"
#include "api/include/models/tool.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_tool(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;
                if (user_id.empty()) {
                    return crow::response(400, "User ID is required");
                }

                UserTool tool;
                tool.user_id = user_id;
                tool.folder_id = request_body.value("folder_id", 0);
                tool.name = request_body.value("name", "tool");
                tool.sharing = request_body.value("sharing", "private");
                tool.description = request_body.value("description", "Default tool");
                tool.created_at = get_current_time();
                tool.updated_at = get_current_time();
                tool.schema = request_body.value("schema", json::object());
                tool.url = request_body.value("url", "");

                models::Tool::create_tool(tool);

                json response = {
                        {"message", "Tool created"}
                };
                return crow::response(201, response.dump());
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create tool: " << e.what();
            return crow::response(400, "During create tool: " + std::string(e.what()));
        }
    }

    crow::response get_tools(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;

                if (user_id.empty()) {
                    return crow::response(400, "User ID must be provided");
                }

                std::vector<UserTool> tools = models::Tool::tools(user_id);

                json tools_json = json::array();
                for (const auto &tool: tools) {
                    json tool_json;
                    models::Tool::to_json(tool_json, tool);
                    tools_json.push_back(tool_json);
                }

                return crow::response(200, tools_json.dump());
            }
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_tools: " << e.what();
            return crow::response(500, "Error during get_tools: " + std::string(e.what()));
        }
    }

    crow::response delete_tool(const crow::request &req, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            if (models::Tool::delete_tool(tool_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete tool: " << e.what();
            return crow::response(500, "Error during delete tool: " + std::string(e.what()));
        }
    }

    crow::response update_tool(const crow::request &req, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserTool tool = models::Tool::get_tool_by_id(tool_id);

            if (tool.user_id.empty()) {
                return crow::response(404, "Tool not found");
            }

            tool.name = request_body.value("name", tool.name);
            tool.sharing = request_body.value("sharing", tool.sharing);
            tool.description = request_body.value("description", tool.description);
            tool.folder_id = request_body.value("folder_id", tool.folder_id);
            tool.schema = request_body.value("schema", tool.schema);
            tool.url = request_body.value("url", tool.url);
            tool.updated_at = get_current_time();

            if (models::Tool::update_tool(tool_id, tool)) {
                return crow::response(200, "tool updated");
            } else {
                return crow::response(400, "tool not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update tool: " << e.what();
            return crow::response(500, "Error during update tool: " + std::string(e.what()));
        }
    }

    crow::response get_tool_by_id(const crow::request &req, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserTool tool = models::Tool::get_tool_by_id(tool_id);

        if (tool.user_id.empty()) {
            return crow::response(404, "tool not found");
        }

        json response;
        models::Tool::to_json(response, tool);

        return crow::response(200, response.dump());
    }

    crow::response get_tools_by_workspace_id(const crow::request &req, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }
        try {
            json request_body = json::parse(req.body);

            if (verify_jwt(auth_header)) {
                auto user_id_opt = get_user_id_from_token(auth_header);

                // Extract user ID to get the profile
                std::string user_id = *user_id_opt;

                if (user_id.empty()) {
                    return crow::response(400, "User ID must be provided");
                }

                std::vector<UserTool> tools = models::Tool::get_tools_by_workspace_id(workspace_id);

                json tools_json = json::array();
                for (const auto &tool: tools) {
                    json tool_json;
                    models::Tool::to_json(tool_json, tool);
                    tools_json.push_back(tool_json);
                }

                return crow::response(200, tools_json.dump());
            }
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_tools: " << e.what();
            return crow::response(500, "Error during get_tools: " + std::string(e.what()));
        }
    }

}

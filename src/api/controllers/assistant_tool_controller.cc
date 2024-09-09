#include "api/include/controllers/assistant_tool_controller.h"
#include "api/include/models/assistant_tool.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_assistant_tool(const crow::request &req) {
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

            UserAssistantTool assistant_tool;
            assistant_tool.user_id = user_id;
            assistant_tool.assistant_id = request_body.value("assistant_id", 0);
            assistant_tool.tool_id = request_body.value("tool_id", 0);
            assistant_tool.created_at = get_current_time();
            assistant_tool.updated_at = get_current_time();

            models::AssistantTool::create_assistant_tool(assistant_tool);

            json response = {
                    {"message", "Assistant tool created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create assistant tool: " << e.what();
            return crow::response(400, "During create assistant tool: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_tools(const crow::request &req) {
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

            std::vector<UserAssistantTool> assistant_tools = models::AssistantTool::assistant_tools(user_id);

            json assistant_tools_json = json::array();
            for (const auto &assistant_tool: assistant_tools) {
                json assistant_tool_json;
                models::AssistantTool::to_json(assistant_tool_json, assistant_tool);
                assistant_tools_json.push_back(assistant_tool_json);
            }

            return crow::response(200, assistant_tools_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_assistant_tools: " << e.what();
            return crow::response(500, "Error during get_assistant_tools: " + std::string(e.what()));
        }
    }

    crow::response delete_assistant_tool(const crow::request &req, const int &assistant_id, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::AssistantTool::delete_assistant_tool(assistant_id, tool_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete assistant tool: " << e.what();
            return crow::response(500, "Error during delete assistant tool: " + std::string(e.what()));
        }
    }

    crow::response update_assistant_tool(const crow::request &req, const int &assistant_id, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserAssistantTool assistant_tool = models::AssistantTool::get_assistant_tool_by_id(assistant_id, tool_id);

            if (assistant_tool.user_id.empty()) {
                return crow::response(404, "Assistant tool not found");
            }

            auto get_user_id = get_user_id_from_token(auth_header);
            assistant_tool.user_id = *get_user_id;
            assistant_tool.updated_at = get_current_time();

            if (models::AssistantTool::update_assistant_tool(assistant_id, tool_id, assistant_tool)) {
                return crow::response(200, "assistant tool updated");
            } else {
                return crow::response(400, "assistant tool not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update assistant tool: " << e.what();
            return crow::response(500, "Error during update assistant tool: " + std::string(e.what()));
        }
    }

    crow::response get_assistant_tool_by_id(const crow::request &req, const int &assistant_id, const int &tool_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserAssistantTool assistant_tool = models::AssistantTool::get_assistant_tool_by_id(assistant_id, tool_id);

        if (assistant_tool.user_id.empty()) {
            return crow::response(404, "assistant tool not found");
        }

        json response;
        models::AssistantTool::to_json(response, assistant_tool);

        return crow::response(200, response.dump());
    }
}

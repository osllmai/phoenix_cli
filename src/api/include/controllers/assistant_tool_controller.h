#pragma once

#include "api/include/models/assistant_tool.h"
#include <crow.h>

namespace controllers {
    crow::response create_assistant_tool(const crow::request &req);
    crow::response update_assistant_tool(const crow::request &req, const int &assistant_id, const int &tool_id);
    crow::response delete_assistant_tool(const crow::request &req, const int &assistant_id, const int &tool_id);
    crow::response get_assistant_tool_by_id(const crow::request &req, const int &assistant_id, const int &tool_id);
    crow::response get_assistant_tools_by_assistant_id(const crow::request &req, const int &assistant_id);
    crow::response get_assistant_tools(const crow::request &req);
}

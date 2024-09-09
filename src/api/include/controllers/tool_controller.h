#pragma once

#include "api/include/models/tool.h"
#include <crow.h>

namespace controllers {
    crow::response create_tool(const crow::request &req);
    crow::response update_tool(const crow::request &req, const int &tool_id);
    crow::response delete_tool(const crow::request &req, const int &tool_id);
    crow::response get_tool_by_id(const crow::request &req, const int &tool_id);
    crow::response get_tools(const crow::request &req);
}

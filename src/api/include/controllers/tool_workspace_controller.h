#pragma once

#include "api/include/models/tool_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_tool_workspace(const crow::request &req);
    crow::response update_tool_workspace(const crow::request &req, const int &tool_id, const int &workspace_id);
    crow::response delete_tool_workspace(const crow::request &req, const int &tool_id, const int &workspace_id);
    crow::response get_tool_workspace_by_id(const crow::request &req, const int &tool_id, const int &workspace_id);
    crow::response get_tool_workspaces(const crow::request &req);
}

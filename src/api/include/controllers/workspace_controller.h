
#pragma once

#include "api/include/models/workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_workspace(const crow::request &req);
    crow::response update_workspace(const crow::request &req, const int &workspace_id);
    crow::response delete_workspace(const crow::request &req, const int &workspace_id);
    crow::response get_workspace_by_id(const crow::request &req, const int &workspace_id);
    crow::response get_workspaces(const crow::request &req);
}
#pragma once

#include "api/include/models/preset_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_preset_workspace(const crow::request &req);
    crow::response update_preset_workspace(const crow::request &req, const int &preset_id, const int &workspace_id);
    crow::response delete_preset_workspace(const crow::request &req, const int &preset_id, const int &workspace_id);
    crow::response get_preset_workspace_by_id(const crow::request &req, const int &preset_id, const int &workspace_id);
    crow::response get_preset_workspaces(const crow::request &req);
}

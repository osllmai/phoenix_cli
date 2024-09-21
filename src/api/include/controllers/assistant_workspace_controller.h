#pragma once

#include "api/include/models/assistant_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_assistant_workspace(const crow::request &req);
    crow::response update_assistant_workspace(const crow::request &req, const int &assistant_id, const int &workspace_id);
    crow::response delete_assistant_workspace(const crow::request &req, const int &assistant_id, const int &workspace_id);
    crow::response get_assistant_workspace_by_id(const crow::request &req, const int &assistant_id, const int &workspace_id);
    crow::response get_assistants_workspace_by_id(const crow::request &req, const int &workspace_id);
    crow::response get_assistant_workspaces(const crow::request &req);
}

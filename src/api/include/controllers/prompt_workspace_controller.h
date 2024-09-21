#pragma once

#include "api/include/models/prompt_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_prompt_workspace(const crow::request &req);
    crow::response update_prompt_workspace(const crow::request &req, const int &prompt_id, const int &workspace_id);
    crow::response delete_prompt_workspace(const crow::request &req, const int &prompt_id, const int &workspace_id);
    crow::response get_prompt_workspace_by_id(const crow::request &req, const int &prompt_id, const int &workspace_id);
    crow::response get_prompt_workspaces(const crow::request &req);
}

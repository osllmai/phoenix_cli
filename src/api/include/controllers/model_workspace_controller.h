#pragma once

#include "api/include/models/model_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_model_workspace(const crow::request &req);
    crow::response update_model_workspace(const crow::request &req, const int &model_id, const int &workspace_id);
    crow::response delete_model_workspace(const crow::request &req, const int &model_id, const int &workspace_id);
    crow::response get_model_workspace_by_id(const crow::request &req, const int &model_id, const int &workspace_id);
    crow::response get_model_workspaces(const crow::request &req);
}

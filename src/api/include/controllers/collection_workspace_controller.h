#pragma once

#include "api/include/models/collection_workspace.h"
#include <crow.h>

namespace controllers {
    crow::response create_collection_workspace(const crow::request &req);
    crow::response update_collection_workspace(const crow::request &req, const int &collection_id, const int &workspace_id);
    crow::response delete_collection_workspace(const crow::request &req, const int &collection_id, const int &workspace_id);
    crow::response get_collection_workspace_by_id(const crow::request &req, const int &collection_id, const int &workspace_id);
    crow::response get_collection_workspaces(const crow::request &req);
}

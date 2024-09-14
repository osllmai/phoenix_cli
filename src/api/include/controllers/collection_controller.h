#pragma once

#include "api/include/models/collection.h"
#include <crow.h>

namespace controllers {
    crow::response create_collection(const crow::request &req);
    crow::response update_collection(const crow::request &req, const int &collection_id);
    crow::response delete_collection(const crow::request &req, const int &collection_id);
    crow::response get_collection_by_id(const crow::request &req, const int &collection_id);
    crow::response get_collections_by_workspace_id(const crow::request &req, const int &workspace_id);
    crow::response get_collections(const crow::request &req);
}

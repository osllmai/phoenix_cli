#pragma once

#include "api/include/models/assistant_collection.h"
#include <crow.h>

namespace controllers {
    crow::response create_assistant_collection(const crow::request &req);
    crow::response update_assistant_collection(const crow::request &req, const int &assistant_id, const int &collection_id);
    crow::response delete_assistant_collection(const crow::request &req, const int &assistant_id, const int &collection_id);
    crow::response get_assistant_collection_by_id(const crow::request &req, const int &assistant_id, const int &collection_id);
    crow::response get_assistant_collections(const crow::request &req);
}

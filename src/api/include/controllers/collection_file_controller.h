#pragma once

#include "api/include/models/collection_file.h"
#include <crow.h>

namespace controllers {
    crow::response create_collection_file(const crow::request &req);
    crow::response update_collection_file(const crow::request &req, const int &collection_id, const int &file_id);
    crow::response delete_collection_file(const crow::request &req, const int &collection_id, const int &file_id);
    crow::response get_collection_file_by_id(const crow::request &req, const int &collection_id, const int &file_id);
    crow::response get_collection_files(const crow::request &req);
}

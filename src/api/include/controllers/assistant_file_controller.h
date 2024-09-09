#pragma once

#include "api/include/models/assistant_file.h"
#include <crow.h>

namespace controllers {
    crow::response create_assistant_file(const crow::request &req);
    crow::response update_assistant_file(const crow::request &req, const int &assistant_id, const int &file_id);
    crow::response delete_assistant_file(const crow::request &req, const int &assistant_id, const int &file_id);
    crow::response get_assistant_file_by_id(const crow::request &req, const int &assistant_id, const int &file_id);
    crow::response get_assistant_files(const crow::request &req);
}

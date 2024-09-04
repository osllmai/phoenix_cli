#pragma once

#include "api/include/models/chat_file.h"
#include <crow.h>

namespace controllers {
    crow::response create_chat_file(const crow::request &req);
    crow::response update_chat_file(const crow::request &req, const int &chat_id, const int &file_id);
    crow::response delete_chat_file(const crow::request &req, const int &chat_id, const int &file_id);
    crow::response get_chat_file_by_id(const crow::request &req, const int &chat_id, const int &file_id);
    crow::response get_chat_files(const crow::request &req);
}

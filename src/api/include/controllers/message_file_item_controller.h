#pragma once

#include "api/include/models/message_file_item.h"
#include <crow.h>

namespace controllers {
    crow::response create_message_file_item(const crow::request &req);
    crow::response update_message_file_item(const crow::request &req, const int &message_id, const int &file_item_id);
    crow::response delete_message_file_item(const crow::request &req, const int &message_id, const int &file_item_id);
    crow::response get_message_file_item_by_id(const crow::request &req, const int &message_id, const int &file_item_id);
    crow::response get_message_file_items(const crow::request &req);
}

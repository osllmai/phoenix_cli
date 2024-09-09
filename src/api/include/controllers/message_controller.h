#pragma once

#include "api/include/models/message.h"
#include <crow.h>

namespace controllers {
    crow::response create_message(const crow::request &req);
    crow::response update_message(const crow::request &req, const int &message_id);
    crow::response delete_message(const crow::request &req, const int &message_id);
    crow::response get_message_by_id(const crow::request &req, const int &message_id);
    crow::response get_messages(const crow::request &req);
}

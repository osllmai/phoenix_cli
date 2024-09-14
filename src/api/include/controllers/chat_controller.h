#pragma once

#include "api/include/models/chat.h"
#include <crow.h>

namespace controllers {
    crow::response create_chat(const crow::request &req);
    crow::response update_chat(const crow::request &req, const int &chat_id);
    crow::response delete_chat(const crow::request &req, const int &chat_id);
    crow::response get_chat_by_id(const crow::request &req, const int &chat_id);
    crow::response get_chats(const crow::request &req);
    crow::response get_chats_by_workspace_id(const crow::request &req, const int &workspace_id);
}

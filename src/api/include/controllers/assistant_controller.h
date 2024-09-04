#pragma once

#include "api/include/models/assistant.h"
#include <crow.h>

namespace controllers {
    crow::response create_assistant(const crow::request &req);
    crow::response update_assistant(const crow::request &req, const int &assistant_id);
    crow::response delete_assistant(const crow::request &req, const int &assistant_id);
    crow::response get_assistant_by_id(const crow::request &req, const int &assistant_id);
    crow::response get_assistants(const crow::request &req);
}

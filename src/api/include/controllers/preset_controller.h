#pragma once

#include "api/include/models/preset.h"
#include <crow.h>

namespace controllers {
    crow::response create_preset(const crow::request &req);
    crow::response update_preset(const crow::request &req, const int &preset_id);
    crow::response delete_preset(const crow::request &req, const int &preset_id);
    crow::response get_preset_by_id(const crow::request &req, const int &preset_id);
    crow::response get_presets(const crow::request &req);
}
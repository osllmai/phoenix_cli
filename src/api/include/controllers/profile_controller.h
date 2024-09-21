//
// Created by Amir Kouhkan on 9/2/24.
//

#pragma once

#include <crow.h>
#include "api/include/models/profile.h"

namespace controllers {
    crow::response update_profile(const crow::request &req);
    crow::response get_by_user_id(const crow::request &req);
}
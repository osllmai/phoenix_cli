//
// Created by Amir Kouhkan on 9/1/24.
//

#pragma once

#include <crow.h>

namespace controllers {
    crow::response register_user(const crow::request &req);
    crow::response login(const crow::request &req);
    crow::response refresh(const crow::request &req);
    crow::response reset_password(const crow::request &req);
    crow::response protected_route(const crow::request &req);
}
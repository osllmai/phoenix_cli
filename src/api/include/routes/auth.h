//
// Created by Amir Kouhkan on 9/1/24.
//

#pragma once

#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_auth_routes(crow::App<crow::CORSHandler> &app);
}

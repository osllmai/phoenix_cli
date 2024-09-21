#pragma once

#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_chat_routes(crow::App<crow::CORSHandler> &app);
}

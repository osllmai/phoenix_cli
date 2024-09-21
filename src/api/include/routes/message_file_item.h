#pragma once

#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_message_file_item_routes(crow::App<crow::CORSHandler> &app);
}
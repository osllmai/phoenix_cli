#pragma once

#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_assistant_file_routes(crow::App<crow::CORSHandler> &app);
}
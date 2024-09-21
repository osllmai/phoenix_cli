#include "api/include/routes/streaming.h"
#include "api/include/controllers/streaming_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_streaming_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/stream_tokens")(controllers::stream_tokens);
    }
}

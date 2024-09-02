#include "api/include/routes/streaming.h"
#include "api/include/controllers/streaming_controller.h"
#include <crow.h>

namespace routes {
    void init_streaming_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/stream_tokens")(controllers::stream_tokens);
    }
}

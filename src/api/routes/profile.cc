#include "api/include/routes/profile.h"
#include "api/include/controllers/profile_controller.h"
#include <crow.h>

namespace routes {
    void init_profile_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/profile").methods("PATCH"_method)([](const crow::request &req) {
            return controllers::update_profile(req);
        });

    }
}

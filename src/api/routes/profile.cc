#include "api/include/routes/profile.h"
#include "api/include/controllers/profile_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_profile_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/profile").methods("PATCH"_method)([](const crow::request &req) {
            return controllers::update_profile(req);
        });

        CROW_ROUTE(app, "/profile").methods("GET"_method)([](const crow::request &req){
           return controllers::get_by_user_id(req);
        });

    }
}

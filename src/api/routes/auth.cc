#include "api/include/routes/auth.h"
#include "api/include/controllers/auth_controller.h"
#include "crow/middlewares/cors.h"
#include <crow.h>

namespace routes {
    void init_auth_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/register").methods("POST"_method)([](const crow::request &req) {
            return controllers::register_user(req);
        });

        CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request &req) {
            return controllers::login(req);
        });

        CROW_ROUTE(app, "/refresh").methods("POST"_method)([](const crow::request &req){
            return controllers::refresh(req);
        });

        CROW_ROUTE(app, "/reset_password").methods("PUT"_method)([](const crow::request &req){
            return controllers::reset_password(req);
        });

        CROW_ROUTE(app, "/protected")(controllers::protected_route);
    }
}

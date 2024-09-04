#include "api/include/routes/preset.h"
#include "api/include/controllers/preset_controller.h"

#include <crow.h>

namespace routes {
    void init_preset_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/preset").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_preset(req);
        });

        CROW_ROUTE(app, "/preset/<int>").methods("PATCH"_method)([](const crow::request &req, const int &preset_id) {
            return controllers::update_preset(req, preset_id);
        });

        CROW_ROUTE(app, "/preset/<int>").methods("DELETE"_method)([](const crow::request &req, const int &preset_id) {
            return controllers::delete_preset(req, preset_id);
        });

        CROW_ROUTE(app, "/preset").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_presets(req);
        });

        CROW_ROUTE(app, "/preset/<int>").methods("GET"_method)([](const crow::request &req, const int &preset_id) {
            return controllers::get_preset_by_id(req, preset_id);
        });


    }
}
#include "api/include/routes/assistant.h"
#include "api/include/controllers/assistant_controller.h"
#include <crow.h>

namespace routes {
    void init_assistant_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/assistant").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_assistant(req);
        });

        CROW_ROUTE(app, "/assistant").methods("GET"_method)([](const crow::request &req){
            return controllers::get_assistants(req);
        });

        CROW_ROUTE(app, "/assistant/<int>").methods("DELETE"_method)([](const crow::request &req, const int &assistant_id){
            return controllers::delete_assistant(req, assistant_id);
        });

        CROW_ROUTE(app, "/assistant/<int>").methods("PATCH"_method)([](const crow::request &req, const int &assistant_id){
            return controllers::update_assistant(req, assistant_id);
        });

        CROW_ROUTE(app, "/assistant/<int>").methods("GET"_method)([](const crow::request &req, const int &assistant_id){
            return controllers::get_assistant_by_id(req, assistant_id);
        });
    }
}

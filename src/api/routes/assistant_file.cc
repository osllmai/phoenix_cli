#include "api/include/routes/assistant_file.h"
#include "api/include/controllers/assistant_file_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_assistant_file_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/assistant_file").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_assistant_file(req);
        });

        CROW_ROUTE(app, "/assistant_file").methods("GET"_method)([](const crow::request &req){
            return controllers::get_assistant_files(req);
        });

        CROW_ROUTE(app, "/assistant_file/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &assistant_id, const int &file_id){
            return controllers::delete_assistant_file(req, assistant_id, file_id);
        });

        CROW_ROUTE(app, "/assistant_file/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &assistant_id, const int &file_id){
            return controllers::update_assistant_file(req, assistant_id, file_id);
        });

        CROW_ROUTE(app, "/assistant_file/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &assistant_id, const int &file_id){
            return controllers::get_assistant_file_by_id(req, assistant_id, file_id);
        });
    }
}
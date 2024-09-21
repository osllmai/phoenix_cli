#include "api/include/routes/chat_file.h"
#include "api/include/controllers/chat_file_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_chat_file_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/chat_file").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_chat_file(req);
        });

        CROW_ROUTE(app, "/chat_file").methods("GET"_method)([](const crow::request &req){
            return controllers::get_chat_files(req);
        });

        CROW_ROUTE(app, "/chat_file/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &chat_id, const int &file_id){
            return controllers::delete_chat_file(req, chat_id, file_id);
        });

        CROW_ROUTE(app, "/chat_file/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &chat_id, const int &file_id){
            return controllers::update_chat_file(req, chat_id, file_id);
        });

        CROW_ROUTE(app, "/chat_file/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &chat_id, const int &file_id){
            return controllers::get_chat_file_by_id(req, chat_id, file_id);
        });
    }
}

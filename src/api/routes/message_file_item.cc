#include "api/include/routes/message_file_item.h"
#include "api/include/controllers/message_file_item_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_message_file_item_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/message_file_item").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_message_file_item(req);
        });

        CROW_ROUTE(app, "/message_file_item").methods("GET"_method)([](const crow::request &req){
            return controllers::get_message_file_items(req);
        });

        CROW_ROUTE(app, "/message_file_item/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &message_id, const int &file_item_id){
            return controllers::delete_message_file_item(req, message_id, file_item_id);
        });

        CROW_ROUTE(app, "/message_file_item/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &message_id, const int &file_item_id){
            return controllers::update_message_file_item(req, message_id, file_item_id);
        });

        CROW_ROUTE(app, "/message_file_item/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &message_id, const int &file_item_id){
            return controllers::get_message_file_item_by_id(req, message_id, file_item_id);
        });
    }
}

#include "api/include/controllers/message_controller.h"
#include "crow/middlewares/cors.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_message_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/message").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_message(req);
        });

        CROW_ROUTE(app, "/message").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_messages(req);
        });

        CROW_ROUTE(app, "/message/<int>").methods("DELETE"_method)([](const crow::request &req, const int &message_id) {
            return controllers::delete_message(req, message_id);
        });

        CROW_ROUTE(app, "/message/<int>").methods("PATCH"_method)([](const crow::request &req, const int &message_id) {
            return controllers::update_message(req, message_id);
        });

        CROW_ROUTE(app, "/message/<int>").methods("GET"_method)([](const crow::request &req, const int &message_id) {
            return controllers::get_message_by_id(req, message_id);
        });

        CROW_ROUTE(app, "/message/by-chat/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &chat_id) {
                    return controllers::get_messages_by_chat_id(req, chat_id);
                });
    }
}

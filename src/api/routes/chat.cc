#include "api/include/routes/chat.h"
#include "api/include/controllers/chat_controller.h"
#include <crow.h>

namespace routes {
    void init_chat_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/chat").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_chat(req);
        });

        CROW_ROUTE(app, "/chat").methods("GET"_method)([](const crow::request &req){
            return controllers::get_chats(req);
        });

        CROW_ROUTE(app, "/chat/<int>").methods("DELETE"_method)([](const crow::request &req, const int &chat_id){
            return controllers::delete_chat(req, chat_id);
        });

        CROW_ROUTE(app, "/chat/<int>").methods("PATCH"_method)([](const crow::request &req, const int &chat_id){
            return controllers::update_chat(req, chat_id);
        });

        CROW_ROUTE(app, "/chat/<int>").methods("GET"_method)([](const crow::request &req, const int &chat_id){
            return controllers::get_chat_by_id(req, chat_id);
        });
    }
}

#include "api/include/routes/assistant_collection.h"
#include "api/include/controllers/assistant_collection_controller.h"
#include <crow.h>

namespace routes {
    void init_assistant_collection_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/assistant_collection").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_assistant_collection(req);
        });

        CROW_ROUTE(app, "/assistant_collection").methods("GET"_method)([](const crow::request &req){
            return controllers::get_assistant_collections(req);
        });

        CROW_ROUTE(app, "/assistant_collection/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &assistant_id, const int &collection_id){
            return controllers::delete_assistant_collection(req, assistant_id, collection_id);
        });

        CROW_ROUTE(app, "/assistant_collection/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &assistant_id, const int &collection_id){
            return controllers::update_assistant_collection(req, assistant_id, collection_id);
        });

        CROW_ROUTE(app, "/assistant_collection/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &assistant_id, const int &collection_id){
            return controllers::get_assistant_collection_by_id(req, assistant_id, collection_id);
        });
    }
}

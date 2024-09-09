#include "api/include/routes/collection.h"
#include "api/include/controllers/collection_controller.h"
#include <crow.h>

namespace routes {
    void init_collection_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/collection").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_collection(req);
        });

        CROW_ROUTE(app, "/collection").methods("GET"_method)([](const crow::request &req){
            return controllers::get_collections(req);
        });

        CROW_ROUTE(app, "/collection/<int>").methods("DELETE"_method)([](const crow::request &req, const int &collection_id){
            return controllers::delete_collection(req, collection_id);
        });

        CROW_ROUTE(app, "/collection/<int>").methods("PATCH"_method)([](const crow::request &req, const int &collection_id){
            return controllers::update_collection(req, collection_id);
        });

        CROW_ROUTE(app, "/collection/<int>").methods("GET"_method)([](const crow::request &req, const int &collection_id){
            return controllers::get_collection_by_id(req, collection_id);
        });
    }
}

#include "api/include/routes/collection_file.h"
#include "api/include/controllers/collection_file_controller.h"
#include <crow.h>

namespace routes {
    void init_collection_file_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/collection_file").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_collection_file(req);
        });

        CROW_ROUTE(app, "/collection_file").methods("GET"_method)([](const crow::request &req){
            return controllers::get_collection_files(req);
        });

        CROW_ROUTE(app, "/collection_file/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &collection_id, const int &file_id){
            return controllers::delete_collection_file(req, collection_id, file_id);
        });

        CROW_ROUTE(app, "/collection_file/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &collection_id, const int &file_id){
            return controllers::update_collection_file(req, collection_id, file_id);
        });

        CROW_ROUTE(app, "/collection_file/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &collection_id, const int &file_id){
            return controllers::get_collection_file_by_id(req, collection_id, file_id);
        });
    }
}

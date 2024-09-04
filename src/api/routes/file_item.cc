#include "api/include/routes/file_item.h"
#include "api/include/controllers/file_item_controller.h"
#include <crow.h>

namespace routes {
    void init_file_item_routes(crow::SimpleApp &app) {
        // Create a new FileItem
        CROW_ROUTE(app, "/file_item").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_file_item(req);
        });

        // Get a FileItem by ID
        CROW_ROUTE(app, "/file_item/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &file_item_id) {
                    return controllers::get_file_item_by_id(req, file_item_id);
                });

        // Update a FileItem by ID
        CROW_ROUTE(app, "/file_item/<int>").methods("PATCH"_method)(
                [](const crow::request &req, const int &file_item_id) {
                    return controllers::update_file_item(req, file_item_id);
                });

        // Delete a FileItem by ID
        CROW_ROUTE(app, "/file_item/<int>").methods("DELETE"_method)(
                [](const crow::request &req, const int &file_item_id) {
                    return controllers::delete_file_item(req, file_item_id);
                });

        CROW_ROUTE(app, "/file_item/by_file").methods("POST"_method)([](const crow::request &req) {
            return controllers::get_file_items_by_file(req);
        });

        CROW_ROUTE(app, "/file_item/match_local").methods("POST"_method)([](const crow::request &req) {
            return controllers::match_file_items_local(req);
        });
    }
}


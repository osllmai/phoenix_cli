#include "api/include/routes/file.h"
#include <api/include/controllers/file_controller.h>
#include <crow.h>

namespace routes {
    void init_file_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/file").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_file(req);
        });

        CROW_ROUTE(app, "/file/<int>").methods("PATCH"_method)([](const crow::request &req, const int &file_id) {
            return controllers::update_file(file_id, req);
        });

        CROW_ROUTE(app, "/file/<int>").methods("DELETE"_method)([](const crow::request &req, const int &file_id) {
            return controllers::delete_file(file_id, req);
        });

        CROW_ROUTE(app, "/file/<int>").methods("GET"_method)([](const crow::request &req, const int &file_id) {
            return controllers::get_file_by_id(file_id, req);
        });

        CROW_ROUTE(app, "/file/by_user").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_files_by_user_id(req);
        });

        CROW_ROUTE(app, "/file/by_workspace").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_files_by_folder_id(req);
        });

    }
}

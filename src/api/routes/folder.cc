#include "api/include/routes/folder.h"
#include <api/include/controllers/folder_controller.h>
#include <crow.h>


namespace routes {
    void init_folder_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/folder").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_folder(req);
        });

        CROW_ROUTE(app, "/folder/<int>").methods("PATCH"_method)([](const crow::request &req, const int &folder_id) {
            return controllers::update_folder(folder_id, req);
        });

        CROW_ROUTE(app, "/folder/<int>").methods("DELETE"_method)([](const crow::request &req, const int &folder_id) {
            return controllers::delete_folder(folder_id, req);
        });

        CROW_ROUTE(app, "/folder/<int>").methods("GET"_method)([](const crow::request &req, const int &folder_id) {
            return controllers::get_folders_by_id(folder_id, req);
        });

        CROW_ROUTE(app, "/folder/by_user").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_folders_by_user_id(req);
        });

        CROW_ROUTE(app, "/folder/by_workspace").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_folders_by_workspace_id(req);
        });

    }
}
#include "api/include/routes/collection_workspace.h"
#include "api/include/controllers/collection_workspace_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_collection_workspace_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/collection_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_collection_workspace(req);
        });

        CROW_ROUTE(app, "/collection_workspace").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_collection_workspaces(req);
        });

        CROW_ROUTE(app, "/collection_workspace/<int>/<int>").methods("DELETE"_method)(
                [](const crow::request &req, const int &collection_id, const int &workspace_id) {
                    return controllers::delete_collection_workspace(req, collection_id, workspace_id);
                });

        CROW_ROUTE(app, "/collection_workspace/<int>/<int>").methods("PATCH"_method)(
                [](const crow::request &req, const int &collection_id, const int &workspace_id) {
                    return controllers::update_collection_workspace(req, collection_id, workspace_id);
                });

        CROW_ROUTE(app, "/collection_workspace/<int>/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &collection_id, const int &workspace_id) {
                    return controllers::get_collection_workspace_by_id(req, collection_id, workspace_id);
                });
    }
}

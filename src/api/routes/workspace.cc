#include "api/include/routes/workspace.h"
#include "api/include/controllers/workspace_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_workspace_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_workspace(req);
        });

        CROW_ROUTE(app, "/workspace").methods("GET"_method)([](const crow::request &req){
           return controllers::get_workspaces(req);
        });


        CROW_ROUTE(app, "/workspace/<int>").methods("DELETE"_method)([](const crow::request &req, const int &workspace_id){
            return controllers::delete_workspace(req, workspace_id);
        });

        CROW_ROUTE(app, "/workspace/<int>").methods("PATCH"_method)([](const crow::request &req, const int &workspace_id){
            return controllers::update_workspace(req, workspace_id);
        });

        CROW_ROUTE(app, "/workspace/<int>").methods("GET"_method)([](const crow::request &req, const int &workspace_id){
            return controllers::get_workspace_by_id(req, workspace_id);
        });


    }
}
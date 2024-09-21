#include "api/include/routes/preset_workspace.h"
#include "api/include/controllers/preset_workspace_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_preset_workspace_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/preset_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_preset_workspace(req);
        });

        CROW_ROUTE(app, "/preset_workspace").methods("GET"_method)([](const crow::request &req){
            return controllers::get_preset_workspaces(req);
        });

        CROW_ROUTE(app, "/preset_workspace/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &preset_id, const int &workspace_id){
            return controllers::delete_preset_workspace(req, preset_id, workspace_id);
        });

        CROW_ROUTE(app, "/preset_workspace/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &preset_id, const int &workspace_id){
            return controllers::update_preset_workspace(req, preset_id, workspace_id);
        });

        CROW_ROUTE(app, "/preset_workspace/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &preset_id, const int &workspace_id){
            return controllers::get_preset_workspace_by_id(req, preset_id, workspace_id);
        });
    }
}

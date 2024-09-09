#include "api/include/routes/tool_workspace.h"
#include "api/include/controllers/tool_workspace_controller.h"
#include <crow.h>

namespace routes {
    void init_tool_workspace_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/tool_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_tool_workspace(req);
        });

        CROW_ROUTE(app, "/tool_workspace").methods("GET"_method)([](const crow::request &req){
            return controllers::get_tool_workspaces(req);
        });

        CROW_ROUTE(app, "/tool_workspace/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &tool_id, const int &workspace_id){
            return controllers::delete_tool_workspace(req, tool_id, workspace_id);
        });

        CROW_ROUTE(app, "/tool_workspace/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &tool_id, const int &workspace_id){
            return controllers::update_tool_workspace(req, tool_id, workspace_id);
        });

        CROW_ROUTE(app, "/tool_workspace/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &tool_id, const int &workspace_id){
            return controllers::get_tool_workspace_by_id(req, tool_id, workspace_id);
        });
    }
}

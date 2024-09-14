#include "api/include/routes/tool.h"
#include "api/include/controllers/tool_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_tool_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/tool").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_tool(req);
        });

        CROW_ROUTE(app, "/tool").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_tools(req);
        });

        CROW_ROUTE(app, "/tool/<int>").methods("DELETE"_method)([](const crow::request &req, const int &tool_id) {
            return controllers::delete_tool(req, tool_id);
        });

        CROW_ROUTE(app, "/tool/<int>").methods("PATCH"_method)([](const crow::request &req, const int &tool_id) {
            return controllers::update_tool(req, tool_id);
        });

        CROW_ROUTE(app, "/tool/<int>").methods("GET"_method)([](const crow::request &req, const int &tool_id) {
            return controllers::get_tool_by_id(req, tool_id);
        });

        CROW_ROUTE(app, "/tool/by-workspace/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &workspace_id) {
                    return controllers::get_tools_by_workspace_id(req, workspace_id);
                });
    }
}

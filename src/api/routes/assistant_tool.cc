#include "api/include/routes/assistant_tool.h"
#include "api/include/controllers/assistant_tool_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_assistant_tool_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/assistant_tool").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_assistant_tool(req);
        });

        CROW_ROUTE(app, "/assistant_tool").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_assistant_tools(req);
        });

        CROW_ROUTE(app, "/assistant_tool/<int>/<int>").methods("DELETE"_method)(
                [](const crow::request &req, const int &assistant_id, const int &tool_id) {
                    return controllers::delete_assistant_tool(req, assistant_id, tool_id);
                });

        CROW_ROUTE(app, "/assistant_tool/<int>/<int>").methods("PATCH"_method)(
                [](const crow::request &req, const int &assistant_id, const int &tool_id) {
                    return controllers::update_assistant_tool(req, assistant_id, tool_id);
                });

        CROW_ROUTE(app, "/assistant_tool/<int>/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &assistant_id, const int &tool_id) {
                    return controllers::get_assistant_tool_by_id(req, assistant_id, tool_id);
                });

        CROW_ROUTE(app, "/assistant_tool/by-assistant/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &assistant_id) {
                    return controllers::get_assistant_tools_by_assistant_id(req, assistant_id);
                });
    }
}

#include "api/include/routes/assistant_workspace.h"
#include "api/include/controllers/assistant_workspace_controller.h"
#include <crow.h>
#include "crow/middlewares/cors.h"

namespace routes {
    void init_assistant_workspace_routes(crow::App<crow::CORSHandler> &app) {
        CROW_ROUTE(app, "/assistant_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_assistant_workspace(req);
        });

        CROW_ROUTE(app, "/assistant_workspace").methods("GET"_method)([](const crow::request &req) {
            return controllers::get_assistant_workspaces(req);
        });

        CROW_ROUTE(app, "/assistant_workspace/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &workspace_id) {
                    return controllers::get_assistants_workspace_by_id(req, workspace_id);
                });

        CROW_ROUTE(app, "/assistant_workspace/<int>/<int>").methods("DELETE"_method)(
                [](const crow::request &req, const int &assistant_id, const int &workspace_id) {
                    return controllers::delete_assistant_workspace(req, assistant_id, workspace_id);
                });

        CROW_ROUTE(app, "/assistant_workspace/<int>/<int>").methods("PATCH"_method)(
                [](const crow::request &req, const int &assistant_id, const int &workspace_id) {
                    return controllers::update_assistant_workspace(req, assistant_id, workspace_id);
                });

        CROW_ROUTE(app, "/assistant_workspace/<int>/<int>").methods("GET"_method)(
                [](const crow::request &req, const int &assistant_id, const int &workspace_id) {
                    return controllers::get_assistant_workspace_by_id(req, assistant_id, workspace_id);
                });
    }
}

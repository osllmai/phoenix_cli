#include "api/include/routes/prompt_workspace.h"
#include "api/include/controllers/prompt_workspace_controller.h"
#include <crow.h>

namespace routes {
    void init_prompt_workspace_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/prompt_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_prompt_workspace(req);
        });

        CROW_ROUTE(app, "/prompt_workspace").methods("GET"_method)([](const crow::request &req){
            return controllers::get_prompt_workspaces(req);
        });

        CROW_ROUTE(app, "/prompt_workspace/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &prompt_id, const int &workspace_id){
            return controllers::delete_prompt_workspace(req, prompt_id, workspace_id);
        });

        CROW_ROUTE(app, "/prompt_workspace/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &prompt_id, const int &workspace_id){
            return controllers::update_prompt_workspace(req, prompt_id, workspace_id);
        });

        CROW_ROUTE(app, "/prompt_workspace/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &prompt_id, const int &workspace_id){
            return controllers::get_prompt_workspace_by_id(req, prompt_id, workspace_id);
        });
    }
}

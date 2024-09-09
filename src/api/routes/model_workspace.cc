#include "api/include/routes/model_workspace.h"
#include "api/include/controllers/model_workspace_controller.h"
#include <crow.h>

namespace routes {
    void init_model_workspace_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/model_workspace").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_model_workspace(req);
        });

        CROW_ROUTE(app, "/model_workspace").methods("GET"_method)([](const crow::request &req){
            return controllers::get_model_workspaces(req);
        });

        CROW_ROUTE(app, "/model_workspace/<int>/<int>").methods("DELETE"_method)([](const crow::request &req, const int &model_id, const int &workspace_id){
            return controllers::delete_model_workspace(req, model_id, workspace_id);
        });

        CROW_ROUTE(app, "/model_workspace/<int>/<int>").methods("PATCH"_method)([](const crow::request &req, const int &model_id, const int &workspace_id){
            return controllers::update_model_workspace(req, model_id, workspace_id);
        });

        CROW_ROUTE(app, "/model_workspace/<int>/<int>").methods("GET"_method)([](const crow::request &req, const int &model_id, const int &workspace_id){
            return controllers::get_model_workspace_by_id(req, model_id, workspace_id);
        });
    }
}

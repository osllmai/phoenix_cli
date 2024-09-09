#include "api/include/routes/model.h"
#include "api/include/controllers/model_controller.h"
#include <crow.h>

namespace routes {
    void init_model_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/model").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_model(req);
        });

        CROW_ROUTE(app, "/model").methods("GET"_method)([](const crow::request &req){
            return controllers::get_models(req);
        });

        CROW_ROUTE(app, "/model/<int>").methods("DELETE"_method)([](const crow::request &req, const int &model_id){
            return controllers::delete_model(req, model_id);
        });

        CROW_ROUTE(app, "/model/<int>").methods("PATCH"_method)([](const crow::request &req, const int &model_id){
            return controllers::update_model(req, model_id);
        });

        CROW_ROUTE(app, "/model/<int>").methods("GET"_method)([](const crow::request &req, const int &model_id){
            return controllers::get_model_by_id(req, model_id);
        });
    }
}

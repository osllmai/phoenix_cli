#include "api/include/routes/prompt.h"
#include "api/include/controllers/prompt_controller.h"
#include <crow.h>

namespace routes {
    void init_prompt_routes(crow::SimpleApp &app) {
        CROW_ROUTE(app, "/prompt").methods("POST"_method)([](const crow::request &req) {
            return controllers::create_prompt(req);
        });

        CROW_ROUTE(app, "/prompt").methods("GET"_method)([](const crow::request &req){
            return controllers::get_prompts(req);
        });

        CROW_ROUTE(app, "/prompt/<int>").methods("DELETE"_method)([](const crow::request &req, const int &prompt_id){
            return controllers::delete_prompt(req, prompt_id);
        });

        CROW_ROUTE(app, "/prompt/<int>").methods("PATCH"_method)([](const crow::request &req, const int &prompt_id){
            return controllers::update_prompt(req, prompt_id);
        });

        CROW_ROUTE(app, "/prompt/<int>").methods("GET"_method)([](const crow::request &req, const int &prompt_id){
            return controllers::get_prompt_by_id(req, prompt_id);
        });
    }
}

#include "api/include/controllers/model_controller.h"
#include "api/include/models/model.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace controllers {
    crow::response create_model(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            UserModel model;
            model.user_id = user_id;
            model.folder_id = request_body.value("folder_id", 0);
            model.api_key = request_body.value("api_key", "");
            model.base_url = request_body.value("base_url", "");
            model.description = request_body.value("description", "");
            model.model_id = request_body.value("model_id", "");
            model.name = request_body.value("name", "");
            model.created_at = get_current_time();
            model.updated_at = get_current_time();

            models::Model::create_model(model);

            json response = {
                    {"message", "Model created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create model: " << e.what();
            return crow::response(400, "During create model: " + std::string(e.what()));
        }
    }

    crow::response get_models(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<UserModel> models = models::Model::models(user_id);

            json models_json = json::array();
            for (const auto &model: models) {
                json model_json;
                models::Model::to_json(model_json, model);
                models_json.push_back(model_json);
            }

            return crow::response(200, models_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during get_models: " << e.what();
            return crow::response(500, "Error during get_models: " + std::string(e.what()));
        }
    }

    crow::response delete_model(const crow::request &req, const int &model_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::Model::delete_model(model_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(404, "not found");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during delete model: " << e.what();
            return crow::response(500, "Error during delete model: " + std::string(e.what()));
        }
    }

    crow::response update_model(const crow::request &req, const int &model_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserModel model = models::Model::get_model_by_id(model_id);

            if (model.user_id.empty()) {
                return crow::response(404, "Model not found");
            }

            auto get_user_id = get_user_id_from_token(auth_header);

            model.user_id = *get_user_id;
            model.folder_id = request_body.value("folder_id", model.folder_id);
            model.api_key = request_body.value("api_key", model.api_key);
            model.base_url = request_body.value("base_url", model.base_url);
            model.description = request_body.value("description", model.description);
            model.model_id = request_body.value("model_id", model.model_id);
            model.name = request_body.value("name", model.name);
            model.updated_at = get_current_time();

            if (models::Model::update_model(model_id, model)) {
                return crow::response(200, "model updated");
            } else {
                return crow::response(400, "model not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during update model: " << e.what();
            return crow::response(500, "Error during update model: " + std::string(e.what()));
        }
    }

    crow::response get_model_by_id(const crow::request &req, const int &model_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        UserModel model = models::Model::get_model_by_id(model_id);

        if (model.user_id.empty()) {
            return crow::response(404, "model not found");
        }

        json response;
        models::Model::to_json(response, model);

        return crow::response(200, response.dump());
    }
}

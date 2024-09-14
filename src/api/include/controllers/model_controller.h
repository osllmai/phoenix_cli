#pragma once

#include "api/include/models/model.h"
#include <crow.h>

namespace controllers {
    crow::response create_model(const crow::request &req);

    crow::response update_model(const crow::request &req, const int &model_id);

    crow::response delete_model(const crow::request &req, const int &model_id);

    crow::response get_model_by_id(const crow::request &req, const int &model_id);

    crow::response get_models(const crow::request &req);

    crow::response get_models_by_workspace_id(const crow::request &req, const int &workspace_id);
}

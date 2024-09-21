#pragma once

#include "api/include/models/prompt.h"
#include <crow.h>

namespace controllers {
    crow::response create_prompt(const crow::request &req);

    crow::response update_prompt(const crow::request &req, const int &prompt_id);

    crow::response delete_prompt(const crow::request &req, const int &prompt_id);

    crow::response get_prompt_by_id(const crow::request &req, const int &prompt_id);

    crow::response get_prompts(const crow::request &req);

    crow::response get_prompts_by_workspace_id(const crow::request &req, const int &workspace_id);
}

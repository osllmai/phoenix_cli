#pragma once

#include "api/include/models/folder.h"
#include <crow.h>

namespace controllers {
    crow::response create_folder(const crow::request &req);

    crow::response update_folder(const int &folder_id, const crow::request &req);

    crow::response delete_folder(const int &folder_id, const crow::request &req);

    crow::response get_folders_by_id(const int &folder_id, const crow::request &req);

    crow::response get_folders_by_user_id(const crow::request &req);

    crow::response get_folders_by_workspace_id(const crow::request &req);


}

#pragma once

#include "api/include/models/folder.h"

#include <crow.h>


namespace controllers {
    crow::response create_file(const crow::request &req);

    crow::response update_file(const int &file_id, const crow::request &req);

    crow::response delete_file(const int &file_id, const crow::request &req);

    crow::response get_file_by_id(const int &file_id, const crow::request &req);

    crow::response get_files_by_user_id(const crow::request &req);

    crow::response get_files_by_folder_id(const crow::request &req);

}
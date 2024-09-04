#pragma once


#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace controllers {
    crow::response create_file_item(const crow::request &req);

    crow::response get_file_item_by_id(const crow::request &req, const int &file_item_id);

    crow::response update_file_item(const crow::request &req, const int &file_item_id);

    crow::response delete_file_item(const crow::request &req, const int &file_item_id);

    crow::response get_file_items_by_file(const crow::request &req);

    crow::response match_file_items_local(const crow::request &req);

}
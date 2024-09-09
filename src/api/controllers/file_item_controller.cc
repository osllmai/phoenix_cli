#include "api/include/controllers/file_item_controller.h"
#include "api/include/models/file_item.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

std::vector<float> deserialize_vector(const std::string &str) {
    std::istringstream iss(str);
    return std::vector<float>(std::istream_iterator<float>(iss), std::istream_iterator<float>());
}

namespace controllers {

    // Create a new FileItem
    crow::response create_file_item(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            auto get_user_id = get_user_id_from_token(auth_header);

            UserFileItem file_item;
            file_item.user_id = *get_user_id;
            file_item.file_id = request_body.at("file_id").get<int>();
            file_item.sharing = request_body.value("sharing", "private");
            file_item.content = request_body.at("content").get<std::string>();
            file_item.tokens = request_body.at("tokens").get<int>();

            // Extracting vectors from JSON (assuming they are arrays of floats)
            file_item.local_embedding = request_body.at("local_embedding").get<std::vector<float>>();
            file_item.openai_embedding = request_body.at("openai_embedding").get<std::vector<float>>();

            models::FileItem::create_file_item(file_item);

            return crow::response(201, "File item created");
        } catch (const json::exception &e) {
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (const std::exception &e) {
            return crow::response(500, "Server error: " + std::string(e.what()));
        }
    }

    // Get a FileItem by ID
    crow::response get_file_item_by_id(const crow::request &req, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserFileItem file_item = models::FileItem::get_file_item_by_id(file_item_id);

            if (file_item.id == 0) {
                return crow::response(404, "File item not found");
            }

            json response;
            models::FileItem::to_json(response, file_item);

            return crow::response(200, response.dump());
        } catch (const std::exception &e) {
            return crow::response(500, "Server error: " + std::string(e.what()));
        }
    }

    // Update a FileItem by ID
    crow::response update_file_item(const crow::request &req, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserFileItem file_item = models::FileItem::get_file_item_by_id(file_item_id);

            if (file_item.id == 0) {
                return crow::response(404, "File item not found");
            }

            auto get_user_id = get_user_id_from_token(auth_header);

            file_item.file_id = request_body.value("file_id", file_item.file_id);
            file_item.user_id = *get_user_id;
            file_item.sharing = request_body.value("sharing", file_item.sharing);
            file_item.content = request_body.value("content", file_item.content);
            file_item.tokens = request_body.value("tokens", file_item.tokens);

            // Extracting vectors from JSON (assuming they are arrays of floats)
            if (request_body.contains("local_embedding")) {
                file_item.local_embedding = request_body.at("local_embedding").get<std::vector<float>>();
            }
            if (request_body.contains("openai_embedding")) {
                file_item.openai_embedding = request_body.at("openai_embedding").get<std::vector<float>>();
            }

            if (models::FileItem::update_file_item(file_item_id, file_item)) {
                return crow::response(200, "File item updated");
            } else {
                return crow::response(400, "Failed to update file item");
            }
        } catch (const json::exception &e) {
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (const std::exception &e) {
            return crow::response(500, "Server error: " + std::string(e.what()));
        }
    }

    // Delete a FileItem by ID
    crow::response delete_file_item(const crow::request &req, const int &file_item_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            if (models::FileItem::delete_file_item(file_item_id)) {
                return crow::response(204);  // No Content
            } else {
                return crow::response(404, "File item not found");
            }
        } catch (const std::exception &e) {
            return crow::response(500, "Server error: " + std::string(e.what()));
        }
    }

    crow::response get_file_items_by_file(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            int file_id = std::stoi(req.url_params.get("file_id"));
            auto file_items = models::FileItem::file_items_by_file(file_id);

            // Convert the vector of UserFileItem to a JSON array
            nlohmann::json response_json;
            response_json["file_items"] = nlohmann::json::array();
            for (const auto &item: file_items) {
                nlohmann::json j;
                models::FileItem::to_json(j, item);
                response_json["file_items"].push_back(j);
            }

            return crow::response{response_json.dump()};
        } catch (std::exception &e) {
            CROW_LOG_ERROR << e.what();
            return crow::response(400, std::string(e.what()));
        }
    }

    crow::response match_file_items_local(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            std::string query_embedding_str = req.url_params.get("query_embedding");
            int match_count = std::stoi(req.url_params.get("match_count"));
            std::vector<int> file_ids;

            // Parse file_ids from the query parameter
            std::istringstream iss(req.url_params.get("file_ids"));
            std::copy(std::istream_iterator<int>(iss),
                      std::istream_iterator<int>(),
                      std::back_inserter(file_ids));

            // Convert query_embedding_str to vector<float>
            std::vector<float> query_embedding = deserialize_vector(query_embedding_str);

            auto file_items = models::FileItem::match_file_items_local(query_embedding, match_count, file_ids);

            // Convert the vector of UserFileItem to a JSON array
            nlohmann::json response_json;
            response_json["file_items"] = nlohmann::json::array();
            for (const auto &item: file_items) {
                nlohmann::json j;
                models::FileItem::to_json(j, item);
                response_json["file_items"].push_back(j);
            }

            return crow::response{response_json.dump()};
        } catch (std::exception &e) {
            CROW_LOG_ERROR << e.what();
            return crow::response(400, std::string(e.what()));
        }
    }
}


#include "api/include/controllers/folder_controller.h"
#include "api/include/models/folder.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace controllers {
    crow::response create_folder(const crow::request &req) {
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
            int workspace_id = request_body.value("workspace_id", 0);

            if (user_id.empty() || workspace_id == 0) {
                return crow::response(400, "User id or workspace id must set");
            }

            UserFolder user_folder;
            user_folder.user_id = user_id;
            user_folder.workspace_id = workspace_id;
            user_folder.name = request_body.value("name", "");
            user_folder.description = request_body.value("description", "");
            user_folder.type = request_body.value("type", "");

            models::Folder::create_folder(user_folder);

            return crow::response(201, "user folder created");


        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During create folder: " << e.what();
            return crow::response(400, "During create folder: " + std::string(e.what()));
        }

    }

    crow::response update_folder(const int &folder_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserFolder user_folder = models::Folder::get_folder_by_id(folder_id);

            if (user_folder.user_id.empty()) {
                return crow::response(404, "folder not found");
            }

            user_folder.type = request_body.value("type", user_folder.type);
            user_folder.description = request_body.value("description", user_folder.description);
            user_folder.name = request_body.value("name", user_folder.name);
            user_folder.workspace_id = request_body.value("workspace_id", user_folder.workspace_id);

            if (models::Folder::update_folder(folder_id, user_folder)) {
                return crow::response(200, "folder updated");
            } else {
                return crow::response(400, "bad request");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During update folder: " << e.what();
            return crow::response(400, "During update folder: " + std::string(e.what()));
        }
    }

    crow::response delete_folder(const int &folder_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserFolder user_folder = models::Folder::get_folder_by_id(folder_id);

            if (user_folder.user_id.empty()) {
                return crow::response(404, "folder not found");
            }

            if (models::Folder::delete_folder(folder_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(400, "bad request");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During update folder: " << e.what();
            return crow::response(400, "During update folder: " + std::string(e.what()));
        }
    }

    crow::response get_folders_by_id(const int &folder_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserFolder user_folder = models::Folder::get_folder_by_id(folder_id);

            if (user_folder.user_id.empty()) {
                return crow::response(404, "folder not found");
            }

            json response;
            models::Folder::to_json(response, user_folder);

            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During update folder: " << e.what();
            return crow::response(400, "During update folder: " + std::string(e.what()));
        }
    }

    crow::response get_folders_by_user_id(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            std::vector<UserFolder> folders;

            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(422, "User id must sent");
            }

            std::vector<UserFolder> user_folders = models::Folder::user_folders_by_user(user_id);

            if (user_folders.empty()) {
                return crow::response(404, "folder not found");
            }

            json response = json::array();
            for (const auto &folder: user_folders) {
                json f;
                models::Folder::to_json(f, folder);
                response.push_back(f);
            }
            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During update folder: " << e.what();
            return crow::response(400, "During update folder: " + std::string(e.what()));
        }
    }

    crow::response get_folders_by_workspace_id(const crow::request &req, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            std::vector<UserFolder> folders;
            std::vector<UserFolder> user_folders = models::Folder::user_folders_by_workspace(workspace_id);

            if (user_folders.empty()) {
                return crow::response(404, "folder not found");
            }

            json response = json::array();
            for (const auto &folder: user_folders) {
                json f;
                models::Folder::to_json(f, folder);
                response.push_back(f);
            }
            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "During update folder: " << e.what();
            return crow::response(400, "During update folder: " + std::string(e.what()));
        }
    }
}
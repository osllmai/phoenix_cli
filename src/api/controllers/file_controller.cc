#include "api/include/controllers/file_controller.h"
#include "api/include/models/file.h"
#include "api/include/utils/utils.h"

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace controllers {

    crow::response create_file(const crow::request &req) {
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

            int folder_id = request_body.value("folder_id", 0);

            if (user_id.empty() || folder_id == 0) {
                return crow::response(400, "User ID or Folder ID must be set");
            }

            UserFile user_file;
            user_file.user_id = user_id;
            user_file.folder_id = folder_id;
            user_file.name = request_body.value("name", "");
            user_file.description = request_body.value("description", "");
            user_file.file_path = request_body.value("file_path", "");
            user_file.sharing = request_body.value("sharing", "private");
            user_file.size = request_body.value("size", 0);
            user_file.tokens = request_body.value("tokens", 0);
            user_file.type = request_body.value("type", "");

            models::File::create_file(user_file);

            return crow::response(201, "User file created");

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file creation: " << e.what();
            return crow::response(400, "Error during file creation: " + std::string(e.what()));
        }
    }

    crow::response update_file(const int &file_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);

            UserFile user_file = models::File::get_file_by_id(file_id);

            if (user_file.user_id.empty()) {
                return crow::response(404, "File not found");
            }

            user_file.name = request_body.value("name", user_file.name);
            user_file.description = request_body.value("description", user_file.description);
            user_file.file_path = request_body.value("file_path", user_file.file_path);
            user_file.sharing = request_body.value("sharing", user_file.sharing);
            user_file.size = request_body.value("size", user_file.size);
            user_file.tokens = request_body.value("tokens", user_file.tokens);
            user_file.type = request_body.value("type", user_file.type);

            if (models::File::update_file(file_id, user_file)) {
                return crow::response(200, "File updated");
            } else {
                return crow::response(400, "Bad request");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file update: " << e.what();
            return crow::response(400, "Error during file update: " + std::string(e.what()));
        }
    }

    crow::response delete_file(const int &file_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserFile user_file = models::File::get_file_by_id(file_id);

            if (user_file.user_id.empty()) {
                return crow::response(404, "File not found");
            }

            if (models::File::delete_file(file_id)) {
                return crow::response(204, "");
            } else {
                return crow::response(400, "Bad request");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file deletion: " << e.what();
            return crow::response(400, "Error during file deletion: " + std::string(e.what()));
        }
    }

    crow::response get_file_by_id(const int &file_id, const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            UserFile user_file = models::File::get_file_by_id(file_id);

            if (user_file.user_id.empty()) {
                return crow::response(404, "File not found");
            }

            json response;
            models::File::to_json(response, user_file);

            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file retrieval: " << e.what();
            return crow::response(400, "Error during file retrieval: " + std::string(e.what()));
        }
    }

    crow::response get_files_by_user_id(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            auto get_user_id = get_user_id_from_token(auth_header);
            std::string user_id = *get_user_id;

            if (user_id.empty()) {
                return crow::response(422, "User ID must be sent");
            }

            std::vector<UserFile> user_files = models::File::files_by_user(user_id);

            if (user_files.empty()) {
                return crow::response(404, "Files not found");
            }

            json response = json::array();
            for (const auto &file: user_files) {
                json f;
                models::File::to_json(f, file);
                response.push_back(f);
            }
            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file retrieval: " << e.what();
            return crow::response(400, "Error during file retrieval: " + std::string(e.what()));
        }
    }

    crow::response get_files_by_folder_id(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);
            int folder_id = request_body.value("folder_id", 0);

            if (folder_id == 0) {
                return crow::response(422, "Workspace ID must be sent");
            }

            std::vector<UserFile> user_files = models::File::files_by_folder(folder_id);

            if (user_files.empty()) {
                return crow::response(404, "Files not found");
            }

            json response = json::array();
            for (const auto &file: user_files) {
                json f;
                models::File::to_json(f, file);
                response.push_back(f);
            }
            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file retrieval: " << e.what();
            return crow::response(400, "Error during file retrieval: " + std::string(e.what()));
        }
    }

    crow::response get_files_by_workspace_id(const crow::request &req, const int &workspace_id) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        try {
            json request_body = json::parse(req.body);
            int folder_id = request_body.value("folder_id", 0);

            if (folder_id == 0) {
                return crow::response(422, "Workspace ID must be sent");
            }

            std::vector<UserFile> user_files = models::File::get_files_by_workspace_id(workspace_id);

            if (user_files.empty()) {
                return crow::response(404, "Files not found");
            }

            json response = json::array();
            for (const auto &file: user_files) {
                json f;
                models::File::to_json(f, file);
                response.push_back(f);
            }
            return crow::response(200, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error during file retrieval: " << e.what();
            return crow::response(400, "Error during file retrieval: " + std::string(e.what()));
        }
    }

}

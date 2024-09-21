#include "api/include/models/model.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <iostream>

namespace models {
    void Model::create_model(const UserModel &model) {
        db << R"(INSERT INTO models (id, user_id, folder_id, created_at, updated_at, sharing, api_key, base_url, description, model_id, name)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)"
           << model.id
           << model.user_id
           << model.folder_id
           << model.created_at
           << model.updated_at
           << model.sharing
           << model.api_key
           << model.base_url
           << model.description
           << model.model_id
           << model.name;
    }

    UserModel Model::get_model_by_id(const int &id) {
        UserModel model;
        db << "SELECT * FROM models WHERE id = ?;" << id >>
           [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
               std::string sharing, std::string api_key, std::string base_url, std::string description,
               std::string model_id, std::string name) {
               model = {id, user_id, folder_id, created_at, updated_at, sharing, api_key, base_url, description,
                        model_id, name};
           };
        return model;
    }

    bool Model::update_model(const int &id, const UserModel &model) {
        try {
            db << "UPDATE models SET "
                  "user_id = ?, folder_id = ?, created_at = ?, updated_at = ?, sharing = ?, "
                  "api_key = ?, base_url = ?, description = ?, model_id = ?, name = ? "
                  "WHERE id = ?"
               << model.user_id
               << model.folder_id
               << model.created_at
               << model.updated_at
               << model.sharing
               << model.api_key
               << model.base_url
               << model.description
               << model.model_id
               << model.name
               << id;

            int affected_rows = 0;
            db << "SELECT changes()" >> affected_rows;
            return affected_rows > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating model: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating model: " << e.what() << std::endl;
            return false;
        }
    }

    bool Model::delete_model(const int &id) {
        bool is_deleted = false;
        db << "DELETE FROM models WHERE id = ?;" << id;
        db << "SELECT changes();" >> is_deleted;
        return is_deleted > 0;
    }

    std::vector<UserModel> Model::models(const std::string &user_id) {
        std::vector<UserModel> result;

        try {
            db
                    << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, api_key, base_url, description, model_id, name "
                       "FROM models WHERE user_id = ?;"
                    << user_id
                    >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                           std::string sharing, std::string api_key, std::string base_url, std::string description,
                           std::string model_id, std::string name) {
                        UserModel model{
                                id, user_id, folder_id, created_at, updated_at, sharing, api_key, base_url, description,
                                model_id, name};
                        result.push_back(model);
                    };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving models: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserModel> Model::get_models_by_workspace_id(const int &workspace_id) {
        std::vector<UserModel> result;

        try {
            db << "SELECT m.id, m.user_id, m.folder_id, m.created_at, m.updated_at, m.sharing, m.api_key, m.base_url, "
                  "m.description, m.model_id, m.name "
                  "FROM models m "
                  "JOIN model_workspaces mw ON m.id = mw.model_id "
                  "WHERE mw.workspace_id = ?;"
               << workspace_id
               >> [&](int id, std::string user_id, int folder_id, std::string created_at, std::string updated_at,
                      std::string sharing, std::string api_key, std::string base_url, std::string description,
                      std::string model_id, std::string name) {
                   UserModel model{
                           id, user_id, folder_id, created_at, updated_at, sharing, api_key, base_url, description,
                           model_id, name};
                   result.push_back(model);
               };

        } catch (const std::exception &e) {
            std::cerr << "Error retrieving models: " << e.what() << std::endl;
        }

        return result;
    }

    std::vector<UserLocalModel> Model::get_local_models() {
        std::vector<UserLocalModel> local_models;

        try {
            db << "SELECT * FROM tbl_models;" >> [&](int id, std::string model_name, std::string path) {
                UserLocalModel local_model{id, model_name, path};
                local_models.push_back(local_model);
            };
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        return local_models;
    }

    void Model::to_json(json &j, const UserModel &model) {
        j = json{
                {"id",          model.id},
                {"user_id",     model.user_id},
                {"folder_id",   model.folder_id},
                {"created_at",  model.created_at},
                {"updated_at",  model.updated_at},
                {"sharing",     model.sharing},
                {"api_key",     model.api_key},
                {"base_url",    model.base_url},
                {"description", model.description},
                {"model_id",    model.model_id},
                {"name",        model.name}
        };
    };

    void Model::local_model_to_json(json &j, const UserLocalModel &model) {
        j = json{
                {"id",         model.id},
                {"model_name", model.model_name},
                {"path",       model.path}
        };
    };
}


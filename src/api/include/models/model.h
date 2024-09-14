#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserModel {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    std::string api_key;
    std::string base_url;
    std::string description;
    std::string model_id;
    std::string name;
};

namespace models {
    class Model {
    public:
        static UserModel get_model_by_id(const int &id);

        static void create_model(const UserModel &model);

        static bool update_model(const int &id, const UserModel &model);

        static bool delete_model(const int &id);

        static std::vector<UserModel> models(const std::string &user_id);

        static std::vector<UserModel> get_models_by_workspace_id(const int &workspace_id);

        static void to_json(json &j, const UserModel &model);
    };
}

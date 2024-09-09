#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserModelWorkspace {
    std::string user_id;
    int model_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class ModelWorkspace {
    public:
        static UserModelWorkspace get_model_workspace_by_id(const int &model_id, const int &workspace_id);
        static void create_model_workspace(const UserModelWorkspace &model_workspace);
        static bool update_model_workspace(const int &model_id, const int &workspace_id, const UserModelWorkspace &model_workspace);
        static bool delete_model_workspace(const int &model_id, const int &workspace_id);
        static std::vector<UserModelWorkspace> model_workspaces(const std::string &user_id);
        static void to_json(json &j, const UserModelWorkspace &model_workspace);
    };
}

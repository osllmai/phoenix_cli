#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserPresetWorkspace {
    std::string user_id;
    int preset_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class PresetWorkspace {
    public:
        static UserPresetWorkspace get_preset_workspace_by_id(const int &preset_id, const int &workspace_id);

        static void create_preset_workspace(const UserPresetWorkspace &user_preset_workspace);

        static bool update_preset_workspace(const int &preset_id, const int &workspace_id, const UserPresetWorkspace &user_preset_workspace);

        static bool delete_preset_workspace(const int &preset_id, const int &workspace_id);

        static std::vector<UserPresetWorkspace> preset_workspaces(const std::string &user_id);

        static void to_json(json &j, const UserPresetWorkspace &preset_workspace);
    };
}

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Preset {
    int id;
    std::string user_id;
    int folder_id;
    std::string created_at;
    std::string updated_at;
    std::string sharing;
    int context_length;
    std::string description;
    std::string embeddings_provider;
    bool include_profile_context;
    bool include_workspace_instructions;
    std::string model;
    std::string name;
    std::string prompt;
    double temperature;
};

namespace models {
    class PresetModel {
    public:
        static Preset get_preset_by_id(const int &id);
        static void create_preset(const Preset &preset);
        static bool update_preset(const int &id, const Preset &preset);
        static bool delete_preset(const int &id);
        static std::vector<Preset> get_presets(const std::string &user_id);
        static void to_json(json &j, const Preset &preset);
    };
}
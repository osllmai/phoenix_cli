#include "api/include/controllers/preset_controller.h"
#include "api/include/models/preset.h"
#include <crow.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using json = nlohmann::json;


namespace controllers {

    std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&currentTime);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    crow::response create_preset(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            if (!request_body.is_object()) {
                return crow::response(400, "Invalid JSON");
            }

            std::string user_id = request_body.value("user_id", "");
            if (user_id.empty()) {
                return crow::response(400, "User ID is required");
            }

            Preset preset;
            preset.user_id = user_id;
            preset.folder_id = request_body.value("folder_id", 0);
            preset.sharing = request_body.value("sharing", "private");
            preset.context_length = request_body.value("context_length", 2048);
            preset.description = request_body.value("description", "");
            preset.embeddings_provider = request_body.value("embeddings_provider", "");
            preset.include_profile_context = request_body.value("include_profile_context", false);
            preset.include_workspace_instructions = request_body.value("include_workspace_instructions", false);
            preset.model = request_body.value("model", "");
            preset.name = request_body.value("name", "");
            preset.prompt = request_body.value("prompt", "");
            preset.temperature = request_body.value("temperature", 0.7);
            preset.created_at = get_current_time();
            preset.updated_at = preset.created_at;

            models::PresetModel::create_preset(preset);

            json response = {
                    {"message", "Preset created"}
            };
            return crow::response(201, response.dump());

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error creating preset: " << e.what();
            return crow::response(500, "Error creating preset: " + std::string(e.what()));
        }
    }

    crow::response update_preset(const crow::request &req, const int &preset_id) {
        try {
            json request_body = json::parse(req.body);

            Preset preset = models::PresetModel::get_preset_by_id(preset_id);

            if (preset.user_id.empty()) {
                return crow::response(404, "Preset not found");
            }

            preset.folder_id = request_body.value("folder_id", preset.folder_id);
            preset.sharing = request_body.value("sharing", preset.sharing);
            preset.context_length = request_body.value("context_length", preset.context_length);
            preset.description = request_body.value("description", preset.description);
            preset.embeddings_provider = request_body.value("embeddings_provider", preset.embeddings_provider);
            preset.include_profile_context = request_body.value("include_profile_context",
                                                                preset.include_profile_context);
            preset.include_workspace_instructions = request_body.value("include_workspace_instructions",
                                                                       preset.include_workspace_instructions);
            preset.model = request_body.value("model", preset.model);
            preset.name = request_body.value("name", preset.name);
            preset.prompt = request_body.value("prompt", preset.prompt);
            preset.temperature = request_body.value("temperature", preset.temperature);
            preset.updated_at = get_current_time();

            if (models::PresetModel::update_preset(preset_id, preset)) {
                return crow::response(200, "Preset updated");
            } else {
                return crow::response(400, "Preset not updated");
            }

        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error updating preset: " << e.what();
            return crow::response(500, "Error updating preset: " + std::string(e.what()));
        }
    }

    crow::response delete_preset(const crow::request &req, const int &preset_id) {
        try {
            if (models::PresetModel::delete_preset(preset_id)) {
                return crow::response(204);
            } else {
                return crow::response(404, "Preset not found");
            }
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error deleting preset: " << e.what();
            return crow::response(500, "Error deleting preset: " + std::string(e.what()));
        }
    }

    crow::response get_preset_by_id(const crow::request &req, const int &preset_id) {
        try {
            Preset preset = models::PresetModel::get_preset_by_id(preset_id);

            if (preset.user_id.empty()) {
                return crow::response(404, "Preset not found");
            }

            json response;
            models::PresetModel::to_json(response, preset);

            return crow::response(200, response.dump());
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error getting preset: " << e.what();
            return crow::response(500, "Error getting preset: " + std::string(e.what()));
        }
    }

    crow::response get_presets(const crow::request &req) {
        try {
            json request_body = json::parse(req.body);

            std::string user_id = request_body.value("user_id", "");

            if (user_id.empty()) {
                return crow::response(400, "User ID must be provided");
            }

            std::vector<Preset> presets = models::PresetModel::get_presets(user_id);

            json presets_json = json::array();
            for (const auto &preset: presets) {
                json preset_json;
                models::PresetModel::to_json(preset_json, preset);
                presets_json.push_back(preset_json);
            }

            return crow::response(200, presets_json.dump());
        } catch (json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (std::exception &e) {
            CROW_LOG_ERROR << "Error getting preset: " << e.what();
            return crow::response(500, "Error getting preset: " + std::string(e.what()));
        }

    }
}
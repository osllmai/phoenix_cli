#include "chat_manager.h"
#include "header.h"
#include "directory_manager.h"
#include "database_manager.h"

#include <string>
#include <set>
#include <random>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>

using json = nlohmann::json;

namespace fs = std::filesystem;

std::string ChatManager::generate_unique_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    std::set<std::string> unique_numbers;
    std::string unique_number;

    while (unique_number.length() < 10) {
        std::string temp_number;
        for (int i = 0; i < 10; ++i) {
            temp_number += std::to_string(dis(gen));
        }
        if (unique_numbers.find(temp_number) == unique_numbers.end()) {
            unique_number = temp_number;
            unique_numbers.insert(temp_number);
        }
    }

    return unique_number;
}


bool ChatManager::create_chat_config_file(const std::string &id, chatParams &params) {
    // create chats directory
    DirectoryManager::create_chats_directory();

    // create configs in json object
    json j;
    j["name"] = "Config for Chat ID " + id;
    j["load_params"]["n_ctx"] = params.n_ctx;
    j["load_params"]["n_batch"] = params.n_batch;
    j["load_params"]["seed"] = params.seed;
    j["inference_params"]["n_threads"] = params.n_threads;
    j["inference_params"]["n_predict"] = params.n_predict;
    j["inference_params"]["top_p"] = params.top_p;
    j["inference_params"]["top_k"] = params.top_k;
    j["inference_params"]["temp"] = params.temp;
    j["inference_params"]["repeat_penalty"] = params.repeat_penalty;
    j["inference_params"]["repeat_penalty"] = params.prompt;

    std::string json_str = j.dump(4);

    // write json file
    std::ofstream output_file(DirectoryManager::get_app_home_path() + "/chats/" + id + ".config.chat.json");
    if (output_file.is_open()) {
        output_file << json_str;
        output_file.close();
    } else {
        std::cerr << "Unable to open json file" << std::endl;
        return false;
    }

    return true;
}


std::string
ChatManager::save_chat_history(const std::string &id, const std::string &prompt, const std::string &answer) {
    std::string file_name = DirectoryManager::get_app_home_path() + "/chats/" + id + ".chat.json";

    std::ifstream input_file(file_name);
    json existing_data;

    if (input_file.is_open()) {
        input_file >> existing_data;
        input_file.close();
    }

    if (!existing_data.contains("messages")) {
        existing_data["messages"] = json::array();
    }

    // Append the new messages to the existing data
    existing_data["messages"].push_back({{"role",    "user"},
                                         {"content", prompt}});
    existing_data["messages"].push_back({{"role",    "assistant"},
                                         {"content", answer}});

    std::ofstream output_file(file_name);
    if (output_file.is_open()) {
        output_file << existing_data.dump(4);
        output_file.close();
    } else {
        std::cerr << "Unable to open json file" << std::endl;
        return "";
    }
    return file_name;
}


std::vector<std::string> ChatManager::chat_histories() {
    std::string chat_history_path = DirectoryManager::get_app_home_path() + "/chats/";
    std::vector<std::string> chat_list;

    for (const auto &chat_entry: fs::directory_iterator(chat_history_path)) {
        if (chat_entry.is_regular_file()) {
            // Ensure the filename does not contain ".config."
            std::string filename = chat_entry.path().filename().string();
            if (filename.find(".config.") == std::string::npos) {
                chat_list.push_back(filename);
            }
        }
    }

    return chat_list;
}

json ChatManager::chat_history_conversation(const std::string &path) {
//    std::string chat_history_path = DirectoryManager::get_app_home_path() + "/chats/";
//    for (const auto &chat_entry: fs::directory_iterator(path)) {
//        if (chat_entry.is_regular_file()) {
    std::ifstream input_file(path);
    json existing_data;

    if (input_file.is_open()) {
        input_file >> existing_data;
        input_file.close();
        return existing_data;
    } else {
        std::cerr << "Unable to open file: " << path << std::endl;
    }
//        }
//    }

    std::cerr << "File not found: " << path << std::endl;
    return {};
}
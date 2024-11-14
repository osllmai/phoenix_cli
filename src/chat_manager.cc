#include "chat_manager.h"
#include "header.h"
#include "directory_manager.h"
#include "database_manager.h"
#include "llama.h"

#include <string>
#include <set>
#include <random>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

std::string PhornixChatManager::generate_unique_id() {
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

bool PhornixChatManager::create_chat_config_file(const std::string &id) {
    try {
        DirectoryManager::create_chats_directory();

        PhornixChatManager::json j;
        j["name"] = "Config for Chat ID " + id;

        std::string json_str = j.dump(4);

        std::ofstream output_file(DirectoryManager::get_app_home_path() + "/chats/" + id + ".config.chat.json");
        if (!output_file) {
            std::cerr << "Error: Unable to open JSON config file for writing\n";
            return false;
        }
        output_file << json_str;
        output_file.close();

    } catch (const std::exception &e) {
        std::cerr << "Exception occurred while creating chat config file: " << e.what() << '\n';
        return false;
    }
    return true;
}

std::string PhornixChatManager::save_chat_history(const std::string &id, const std::string &prompt, const std::string &answer) {
    std::string file_name = DirectoryManager::get_app_home_path() + "/chats/" + id + ".chat.json";
    PhornixChatManager::json existing_data;

    try {
        std::ifstream input_file(file_name);
        if (input_file.is_open()) {
            input_file >> existing_data;
            input_file.close();
        }
    } catch (const std::exception &e) {
        std::cerr << "Error reading chat history from file: " << e.what() << '\n';
    }

    if (!existing_data.contains("messages")) {
        existing_data["messages"] = PhornixChatManager::json::array();
    }

    existing_data["messages"].push_back({{"role", "user"}, {"content", prompt}});
    existing_data["messages"].push_back({{"role", "assistant"}, {"content", answer}});

    try {
        std::ofstream output_file(file_name);
        if (!output_file) {
            std::cerr << "Error: Unable to open chat history file for writing\n";
            return "";
        }
        output_file << existing_data.dump(4);
        output_file.close();
    } catch (const std::exception &e) {
        std::cerr << "Exception occurred while saving chat history: " << e.what() << '\n';
        return "";
    }

    return file_name;
}

std::vector<std::string> PhornixChatManager::chat_histories() {
    std::string chat_history_path = DirectoryManager::get_app_home_path() + "/chats/";
    std::vector<std::string> chat_list;

    try {
        for (const auto &chat_entry : fs::directory_iterator(chat_history_path)) {
            if (chat_entry.is_regular_file()) {
                std::string filename = chat_entry.path().filename().string();
                if (filename.find(".config.") == std::string::npos) {
                    chat_list.push_back(filename);
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error while accessing chat histories: " << e.what() << '\n';
    }

    return chat_list;
}

PhornixChatManager::json PhornixChatManager::chat_history_conversation(const std::string &path) {
    PhornixChatManager::json existing_data;

    try {
        std::ifstream input_file(path);
        if (input_file.is_open()) {
            input_file >> existing_data;
            input_file.close();
        } else {
            std::cerr << "Error: Unable to open chat history file: " << path << '\n';
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception occurred while reading chat history conversation: " << e.what() << '\n';
    }

    if (existing_data.empty()) {
        std::cerr << "Warning: Chat history conversation data is empty or invalid for file: " << path << '\n';
    }
    return existing_data;
}

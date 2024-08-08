#include "commands_list.h"
#include "models_list.h"
#include "download_model.h"
#include "directory_manager.h"
#include "chat_manager.h"
#include <nlohmann/json.hpp>
#include <chat.cc>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>

using json = nlohmann::json;
namespace fs = std::filesystem;

void print_help() {
    std::cout << "PhoenixCLI is a large language model runner" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "phoenix [flags]" << std::endl;
    std::cout << "phoenix [command]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  list\t\tList models" << std::endl;
    std::cout << "  run\t\tRun a model" << std::endl;
    std::cout << "  exec\t\tRun a model on your local machine" << std::endl;
    std::cout << "  pull\t\tPull a model from registry" << std::endl;
    std::cout << "  rm\t\tRemove a model" << std::endl;
    std::cout << "  history\tList of chats" << std::endl;
    std::cout << "  help\t\tHelp about any command" << std::endl;
    std::cout << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  -h, --help\tHelp for phoenix" << std::endl;
    std::cout << "  -v, --version\tShow version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Use \"phoenix [command] --help\" for more information about a command." << std::endl;
}

void handle_run_command(const std::string& model_name) {
    json model = model_data(model_name);
    const std::string model_path = DirectoryManager::get_app_home_path() + "/models/" +
                                   model["companyName"].get<std::string>() + "/" + model_name + ".gguf";
    if (!model_path.empty()) {
        run_command(model_path);
    } else {
        std::cerr << "Model not found in the application directory." << std::endl;
    }
}

void handle_pull_command(const std::string& model_name) {
    json model = model_data(model_name);
    if (model.empty()) {
        std::cerr << "Model not found in the list of available models." << std::endl;
        return;
    }
    std::string model_url = get_url_llm_download(model_name);
    if (model_url.empty()) {
        std::cerr << "Model URL not found." << std::endl;
        return;
    }
    DirectoryManager::create_custom_directory(DirectoryManager::get_app_home_path() + "/models",
                                              model["companyName"].get<std::string>());
    const std::string model_path = DirectoryManager::get_app_home_path() + "/models/" +
                                   model["companyName"].get<std::string>() + "/" + model_name + ".gguf";
    if (download_model_file(model_url, model_path)) {
        std::cout << "Model downloaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to complete download model" << std::endl;
    }
}

void handle_list_command(const std::string& option) {
    if (option == "--local") {
        std::cout << "List models which downloaded" << std::endl;
        std::cout << "----------------------------" << std::endl;
        for (const auto& model : DirectoryManager::local_models()) {
            std::cout << model << std::endl;
        }
    } else if (option == "--remote") {
        std::cout << "List models which you can download" << std::endl;
        std::cout << "----------------------------" << std::endl;
        json list_of_models = list_of_models_available();
        for (const auto& [key, value] : list_of_models.items()) {
            std::cout << key << std::endl;
        }
    } else {
        std::cout << "You can see which models located in your local machine or exists on phoenix to download" << std::endl;
        std::cout << std::endl;
        std::cout << "  --local\tList of local machine LLMs" << std::endl;
        std::cout << "  --remote\tList of phoenix LLMs" << std::endl;
    }
}

void handle_exec_command(const std::string& model_path) {
    run_command(model_path);
}

void handle_history_command(const std::string& chat_id) {
    if (chat_id.empty()) {
        std::cout << "List of your chats:" << std::endl;
        std::cout << "----------------------------" << std::endl;
        std::vector<std::string> history = ChatManager::chat_histories();
        for (const auto& chat : history) {
            std::cout << chat << std::endl;
        }
    } else {
        json chat_history = ChatManager::chat_history_conversation(chat_id);
        if (!chat_history.empty()) {
            std::cout << chat_history.dump(4) << std::endl;
        } else {
            std::cout << "No chat history found for ID: " << chat_id << std::endl;
        }
    }
}

void handle_rm_command(const std::string& model_name) {
    if (DirectoryManager::delete_model(model_name)) {
        std::cout << "Model deleted" << std::endl;
    } else {
        std::cout << "Model not found" << std::endl;
    }
}

void show_commands(int argc, char** argv) {
    DirectoryManager::handle_application_directory();
    DirectoryManager::create_custom_directory(DirectoryManager::get_app_home_path(), "models");
    DirectoryManager::create_custom_directory(DirectoryManager::get_app_home_path(), "chats");

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "help" || arg == "--help") {
            print_help();
            return;
        } else if (arg == "run") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help") || argv[i + 1] == std::string("-h")) {
                    std::cout << "Run a model" << std::endl;
                    return;
                }
                handle_run_command(argv[i + 1]);
                return;
            } else {
                std::cout << "Error: 'run' command requires a model argument" << std::endl;
                return;
            }
        } else if (arg == "pull") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help") || argv[i + 1] == std::string("-h")) {
                    std::cout << "Download LLM" << std::endl;
                    return;
                }
                handle_pull_command(argv[i + 1]);
                return;
            }
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "Current version is 0.0.3" << std::endl;
        } else if (arg == "list") {
            if (i + 1 < argc) {
                handle_list_command(argv[i + 1]);
                return;
            } else {
                handle_list_command("");
                return;
            }
        } else if (arg == "exec") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "Run llm on your local machine" << std::endl;
                    std::cout << std::endl;
                    std::cout << "Usage: " << std::endl;
                    std::cout << "  ./phoenix exec /path/to/model.gguf" << std::endl;
                    return;
                }
                handle_exec_command(argv[i + 1]);
            }
        } else if (arg == "history") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "All chat conversations generated in your home" << std::endl;
                    std::cout << "Usage:" << std::endl;
                    std::cout << "  ./phoenix history" << std::endl;
                    std::cout << std::endl;
                    std::cout << "To open history enter chat ID. e.g:" << std::endl;
                    std::cout << "  ./phoenix history [ID.chat.json]" << std::endl;
                    return;
                }
                handle_history_command(argv[i + 1]);
                return;
            } else {
                handle_history_command("");
                return;
            }
        } else if (arg == "rm") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "Remove a model downloaded" << std::endl;
                    std::cout << "Usage:" << std::endl;
                    std::cout << "  ./phoenix rm [model_name]" << std::endl;
                    return;
                }
                handle_rm_command(argv[i + 1]);
                return;
            }
        } else {
            std::cout << "Unknown command. Use --help for usage information." << std::endl;
        }
    }
}

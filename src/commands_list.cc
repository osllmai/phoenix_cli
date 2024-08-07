#include "commands_list.h"
#include "models_list.h"
#include "download_model.h"
#include "directory_manager.h"
#include <nlohmann/json.hpp>

#include <chat.cc>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

void show_commands(int argc, char **argv) {
    DirectoryManager::handle_application_directory();

    json list_of_models = list_of_models_available();

    std::vector<std::string> positional_args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "help" || arg == "--help") {
            std::cout << "PhoenixCLI is a large language model runner" << std::endl;
            std::cout << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "phoenix [flags]" << std::endl;
            std::cout << "phoenix [command]" << std::endl;
            std::cout << std::endl;
            std::cout << "Available Commands:" << std::endl;
            std::cout << "  list\tList models" << std::endl;
            std::cout << "  run\tRun a model" << std::endl;
            std::cout << "  exec\tRun a model on your local machine" << std::endl;
            std::cout << "  pull\tPull a model from registry" << std::endl;
            std::cout << "  help\tHelp about any command" << std::endl;
            std::cout << std::endl;
            std::cout << "Flags:" << std::endl;
            std::cout << "  -h, --help\tHelp for phoenix" << std::endl;
            std::cout << "  -v, --version\tShow version information" << std::endl;
            std::cout << std::endl;
            std::cout << "Use \"phoenix [command] --help\" for more information about a command." << std::endl;
            return;
        } else if (arg == "run") {
            if (i + 1 < argc) {
                if (std::strcmp(argv[i + 1], "--help") == 0 || std::strcmp(argv[i + 1], "-h") == 0) {
                    std::cout << "Run a model" << std::endl;
                    return;
                }
                std::string model = argv[i + 1];
                std::cout << "You want to run " << model << std::endl;
                const std::string run_command_model_path = DirectoryManager::find_llm_in_app_home(model);
                if (!run_command_model_path.empty()) {
                    run_command(run_command_model_path);
                } else {
                    std::cerr << "Model not found in the application directory." << std::endl;
                    return;
                }
                return;
            } else {
                std::cout << "Error: 'run' command requires a model argument" << std::endl;
                return;
            }
        } else if (arg == "pull") {
            if (i + 1 < argc) {
                if (std::strcmp(argv[i + 1], "--help") == 0 || std::strcmp(argv[i + 1], "-h") == 0) {
                    std::cout << "Download LLM" << std::endl;
                    return;
                }
                std::string model_name = argv[i + 1];
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
                std::string models = "models";
                DirectoryManager::create_custom_directory(DirectoryManager::get_app_home_path(), models);
                DirectoryManager::create_custom_directory(DirectoryManager::get_app_home_path() + "/" + models,
                                                          model["companyName"].get<std::string>());
                const std::string model_path =
                        DirectoryManager::get_app_home_path() + "/models/" +
                        model["companyName"].get<std::string>() + "/" + model_name +
                        ".gguf";

                if (download_model_file(model_url, model_path)) {
                    std::cout << "Model downloaded successfully!" << std::endl;
                    return;
                } else {
                    std::cout << "Failed to complete download model" << std::endl;
                    return;
                }
            }
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "Current version is 0.0.3" << std::endl;
            return;
        } else if (arg == "list") {
            if (i + 1 < argc) {
                if (std::strcmp(argv[i + 1], "--local") == 0) {
                    std::cout << "List models which downloaded" << std::endl;
                    std::cout << "----------------------------" << std::endl;
                    for (const auto &model : DirectoryManager::local_models()) {
                        std::cout << model << std::endl;
                    }
                } else if (std::strcmp(argv[i + 1], "--remote") == 0) {
                    std::cout << "List models which you can download" << std::endl;
                    std::cout << "----------------------------" << std::endl;
                    for (const auto &[key, value] : list_of_models.items()) {
                        std::cout << key << std::endl;
                    }
                }
            } else {
                std::cout << "You can see which models located in your local machine or exists on phoenix to download"
                          << std::endl;
                std::cout << std::endl;
                std::cout << "  --local\tList of local machine LLMs" << std::endl;
                std::cout << "  --remote\tList of phoenix LLMs" << std::endl;
            }
            return;
        } else if (arg == "exec") {
            if (i + 1 < argc) {
                if (std::strcmp(argv[i + 1], "--help") == 0) {
                    std::cout << "Run llm on your local machine" << std::endl;
                    std::cout << std::endl;
                    std::cout << "Usage: " << std::endl;
                    std::cout << "  ./phoenix exec /path/to/model.gguf" << std::endl;
                    return;
                }
                run_command(argv[i + 1]);
                return;
            }
        }
    }

    std::cout << "Unknown command. Use --help for usage information." << std::endl;
}

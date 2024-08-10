#include "commands_list.h"
#include "models_list.h"
#include "download_model.h"
#include "directory_manager.h"
#include "chat_manager.h"
#include "web_server.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chat.cc>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <thread>

using json = nlohmann::json;
namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void print_help() {
    std::cout << "PhoenixCLI is a large language model runner developed by osllm.ai. It is open-source software that you can use freely to run and manage large language models." << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "phoenix [flags]" << std::endl;
    std::cout << "phoenix [command]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  serve\t\tStart the phoenix server" << std::endl;
    std::cout << "  list\t\tDisplays a list of all available models that you can run or manage" << std::endl;
    std::cout << "  run\t\tExecutes a model based on your specified input" << std::endl;
    std::cout << "  exec\t\tRuns a model directly on your local machine" << std::endl;
    std::cout << "  pull\t\tRetrieves a model from the registry and stores it locally" << std::endl;
    std::cout << "  rm\t\tRemove a model from your local storage" << std::endl;
    std::cout << "  history\tShows a list of your previous chats" << std::endl;
    std::cout << "  help\t\tProvides detailed information and guidance about any specific command" << std::endl;
    std::cout << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  -h, --help\tDisplays help information for the phoenix command" << std::endl;
    std::cout << "  -v, --version\tOutputs the current version of PhoenixCLI" << std::endl;
    std::cout << std::endl;
    std::cout << "Use \"phoenix [command] --help\" for more information about a command." << std::endl;
}

void handle_run_command(const std::string &model_name) {
    json model = model_data(model_name);
    const std::string model_path = DirectoryManager::get_app_home_path() + "/models/" +
                                   model["companyName"].get<std::string>() + "/" + model_name + ".gguf";
    if (!model_path.empty()) {
        run_command(model_path);
    } else {
        std::cerr << "Model not found in the application directory." << std::endl;
    }
}

void handle_pull_command(const std::string &model_name) {
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

void handle_list_command(const std::string &option) {
    if (option == "--local") {
        std::cout << "List models which downloaded" << std::endl;
        std::cout << "----------------------------" << std::endl;
        for (const auto &model: DirectoryManager::local_models()) {
            std::cout << model << std::endl;
        }
    } else if (option == "--remote") {
        std::cout << "List models which you can download" << std::endl;
        std::cout << "----------------------------" << std::endl;
        json list_of_models = list_of_models_available();
        for (const auto &[key, value]: list_of_models.items()) {
            std::cout << key << std::endl;
        }
    } else {
        std::cout << "You can see which models located in your local machine or exists on phoenix to download"
                  << std::endl;
        std::cout << std::endl;
        std::cout << "  --local\tList of local machine LLMs" << std::endl;
        std::cout << "  --remote\tList of phoenix LLMs" << std::endl;
    }
}

void handle_exec_command(const std::string &model_path) {
    run_command(model_path);
}

void handle_history_command(const std::string &chat_id) {
    if (chat_id.empty()) {
        std::cout << "List of your chats:" << std::endl;
        std::cout << "----------------------------" << std::endl;
        std::vector<std::string> history = ChatManager::chat_histories();
        for (const auto &chat: history) {
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

void handle_rm_command(const std::string &model_name) {
    if (DirectoryManager::delete_model(model_name)) {
        std::cout << "Model deleted" << std::endl;
    } else {
        std::cout << "Model not found" << std::endl;
    }
}

std::string handle_request(tcp::socket& socket, beast::flat_buffer& buffer) {
    // Read the request
    http::request<http::dynamic_body> req;
    http::read(socket, buffer, req);

    // Check if the request is a POST to /api/generate
    if (req.method() == http::verb::post && req.target() == "/api/generate") {
        // Parse the JSON body
        std::string body = beast::buffers_to_string(req.body().data());
        std::istringstream json_stream(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(json_stream, pt);

        // Extract model and prompt from the JSON
        std::string model = pt.get<std::string>("model");
        std::string prompt = pt.get<std::string>("prompt");
        std::string model_name = "/Users/amir/Workspace/models/Meta-Llama-3-8B-Instruct.Q4_0.gguf";


        // Call the API with the extracted values
        std::string api_response = chat_with_api(model_name, prompt);

        // Prepare the JSON response
        boost::property_tree::ptree response_pt;
        response_pt.put("response", api_response);
        std::ostringstream json_response_stream;
        boost::property_tree::write_json(json_response_stream, response_pt);
        std::string json_response = json_response_stream.str();

        // Prepare the HTTP response
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "application/json");
        res.body() = json_response;
        res.prepare_payload();

        // Send the response
        http::write(socket, res);

        return api_response;
    } else {
        // Return an error response for invalid requests
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "Invalid request"})";
        res.prepare_payload();

        // Send the response
        http::write(socket, res);

        return "Invalid request";
    }
}

std::string handle_serv_command() {
    try {
        // Hardcoded address and port
        std::string address = "0.0.0.0";
        unsigned short port = 8080;

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, {net::ip::make_address(address), port}};

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            // Create a buffer for the request
            beast::flat_buffer buffer;

            // Handle the request
            std::string api_response = handle_request(socket, buffer);

            std::cout << "AI >>>>>> " << api_response << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "Error: " + std::string(e.what());
    }
}

void show_commands(int argc, char **argv) {
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
        } else if (arg == "serve") {
            std::cout << "Run server on port :11343" << std::endl;
            std::thread serv_thread(handle_serv_command);
            std::string model_name = "/Users/amir/Workspace/models/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
            std::thread run_model_thread(handle_exec_command, model_name);

            serv_thread.join();
            run_model_thread.join();

        } else {
            std::cout << "Unknown command. Use --help for usage information." << std::endl;
        }
    }
}

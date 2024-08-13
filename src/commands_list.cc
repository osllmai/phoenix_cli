#include "commands_list.h"
#include "models_list.h"
#include "download_model.h"
#include "directory_manager.h"
#include "chat_manager.h"
#include "web_server.h"
#include "database_manager.h"

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chat.cc>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <deque>


namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using json = nlohmann::json;
using tcp = net::ip::tcp;

void print_help() {
    std::cout
            << "PhoenixCLI is a large language model runner developed by osllm.ai. It is open-source software that you can use freely to run and manage large language models."
            << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "phoenix [flags]" << std::endl;
    std::cout << "phoenix [command]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  serve\t\tStart the phoenix server" << std::endl;
    std::cout << "  list\t\tDisplays a list of all available models that you can run or manage" << std::endl;
    std::cout << "  show\t\tShow full information about a model" << std::endl;
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
    sqlite3 *db;
    if (sqlite3_open("phoenix.db", &db) == SQLITE_OK) {
        std::string model_path = DatabaseManager::get_path_by_model_name(db, model_name);
        if (model_path.empty()) {
            std::cerr << "Failed to find model: " << model_name << std::endl;
            sqlite3_close(db);
            return;
        }

        json data = model_data(model_name);
        if (data.contains("promptTemplate") && data["promptTemplate"].is_string()) {
            std::string prompt_template = data["promptTemplate"].get<std::string>();
            sqlite3_close(db);
            run_command(prompt_template, model_path);
        } else {
            std::cerr << "Error: 'promptTemplate' is missing or not a string for model: " << model_name << std::endl;
            sqlite3_close(db);
            return;
        }
    } else {
        std::cerr << "Failed to open database." << std::endl;
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
                                   model["companyName"].get<std::string>() + "/" + model["filename"].get<std::string>();
    sqlite3 *db;

    if (download_model_file(model_url, model_path)) {
        if (sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
            DatabaseManager::insert_models(db, model_name, model_path);
            sqlite3_close(db);
        }
        std::cout << "Model downloaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to complete download model" << std::endl;
    }
}

void handle_list_command(const std::string &option) {
    if (option == "--local") {
        std::cout << "List models which downloaded" << std::endl;
        std::cout << "----------------------------" << std::endl;
        sqlite3 *db;
        if (sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
            DatabaseManager::read_models(db);
            sqlite3_close(db);
        }
//        for (const auto &model: DirectoryManager::local_models()) {
//            std::cout << model << std::endl;
//        }
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
    std::string prompt_template;
    std::cout << "Enter your prompt template" << std::endl;
    std::cout
            << "If your leave blank, default template used:\n <|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>"
            << std::endl;
    std::cout << ">>> ";

    std::getline(std::cin, prompt_template);

    if (prompt_template.empty()) {
        prompt_template = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";
    }
    run_command(prompt_template, model_path);
}

void handle_history_command(const std::string &chat_id) {
    if (chat_id.empty()) {
        std::cout << "List of your chats:" << std::endl;
        std::cout << "----------------------------" << std::endl;
        sqlite3 *db;
        if (sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
            DatabaseManager::read_chat_histories(db);
            sqlite3_close(db);
        }
//        std::vector<std::string> history = ChatManager::chat_histories();
//        for (const auto &chat: history) {
//            std::cout << chat << std::endl;
//        }
    } else {
        std::string chat_file_path;
        sqlite3 *db;
        if (sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
            chat_file_path = DatabaseManager::get_path_by_filename(db, chat_id);
            sqlite3_close(db);
        }
        json chat_history = ChatManager::chat_history_conversation(chat_file_path);
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
        sqlite3 *db;
        if (sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
            DatabaseManager::delete_model(db, model_name);
            sqlite3_close(db);
        }
    } else {
        std::cout << "Model not found" << std::endl;
    }
}


void handle_request(std::shared_ptr<tcp::socket> socket, beast::flat_buffer buffer) {
    try {
        // Read the request
        http::request<http::dynamic_body> req;
        http::read(*socket, buffer, req);

        // Set Keep-Alive
        bool keep_alive = req.keep_alive();

        if (req.method() == http::verb::post && req.target() == "/api/generate") {
            // Parse the JSON body
            std::string body = beast::buffers_to_string(req.body().data());
            std::istringstream json_stream(body);
            boost::property_tree::ptree pt;
            boost::property_tree::read_json(json_stream, pt);

            std::string model_name = pt.get<std::string>("model");
            std::string prompt = pt.get<std::string>("prompt");
            bool stream = pt.get<bool>("stream");

            std::cout << model_name << std::endl;

            json data = model_data(model_name);
            std::string path;
            std::string prompt_template = data["promptTemplate"].get<std::string>();

            sqlite3 *db;
            if (sqlite3_open("phoenix.db", &db) == SQLITE_OK) {
                path = DatabaseManager::get_path_by_model_name(db, model_name);
                std::cout << path << std::endl;
                sqlite3_close(db);
            }

            if (stream) {
                // Prepare the HTTP response with chunked transfer encoding
                http::response<http::empty_body> res{http::status::ok, req.version()};
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "application/json");
                res.set(http::field::transfer_encoding, "chunked");
                res.keep_alive(keep_alive);  // Keep-Alive header

                // Send the headers
                http::serializer<false, http::empty_body> sr{res};
                http::write_header(*socket, sr);

                auto send_chunk = [socket](const std::string &data) {
                    std::string chunk = data;
                    std::stringstream ss;
                    ss << std::hex << chunk.size() << "\r\n" << chunk << "\r\n";
                    std::string formatted_chunk = ss.str();

                    boost::system::error_code ec;
                    boost::asio::write(*socket, boost::asio::buffer(formatted_chunk), ec);
                    if (ec) {
                        std::cerr << "Error sending chunk: " << ec.message() << std::endl;
                        return false;
                    }
                    return true;
                };

                auto token_callback = [&send_chunk](const std::string &token) -> bool {
                    std::string json_chunk = "{\"chunk\": \"" + token + "\"}";
                    return send_chunk(json_chunk);
                };

                chat_with_api_stream(prompt_template, path, prompt, token_callback);

                // Send the final chunk to indicate the end of the stream
                send_chunk("[DONE]");

                // Send the terminating chunk
                boost::system::error_code ec;
                boost::asio::write(*socket, boost::asio::buffer("0\r\n\r\n"), ec);
                if (ec) {
                    std::cerr << "Error sending terminating chunk: " << ec.message() << std::endl;
                }

            } else {
                // Non-streaming response

                std::string api_response = chat_with_api(prompt_template, path, prompt);

                boost::property_tree::ptree response_pt;
                response_pt.put("response", api_response);
                std::ostringstream json_response_stream;
                boost::property_tree::write_json(json_response_stream, response_pt);
                std::string json_response = json_response_stream.str();

                http::response<http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "application/json");
                res.body() = json_response;
                res.prepare_payload();
                res.keep_alive(keep_alive);  // Keep-Alive header

                http::write(*socket, res);
            }
        } else {
            // Return an error response for invalid requests
            http::response<http::string_body> res{http::status::bad_request, req.version()};
            res.set(http::field::server, "Beast");
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"error": "Invalid request"})";
            res.prepare_payload();
            res.keep_alive(keep_alive);  // Keep-Alive header

            http::write(*socket, res);
        }

        if (!keep_alive) {
            socket->shutdown(tcp::socket::shutdown_send);
        }

    } catch (const std::exception &e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
        http::response<http::string_body> res{http::status::internal_server_error, 11};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "Internal server error"})";
        res.prepare_payload();

        http::write(*socket, res);
    }
}

std::string handle_server() {
    try {
        std::string address = "0.0.0.0";
        unsigned short port = 8080;

        net::io_context ioc;
        tcp::acceptor acceptor{ioc, {net::ip::make_address(address), port}};

        std::cout << "Server started on " << address << ":" << port << std::endl;

        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> thread_pool;

        for (int i = 0; i < num_threads; ++i) {
            thread_pool.emplace_back([&ioc]() {
                ioc.run();
            });
        }

        for (;;) {
            auto socket = std::make_shared<tcp::socket>(ioc);
            acceptor.accept(*socket);

            std::cout << "New connection accepted from " << socket->remote_endpoint() << std::endl;

            beast::flat_buffer buffer;

            std::thread{[socket, buffer]() mutable {
                try {
                    handle_request(socket, std::move(buffer));
                } catch (const std::exception &e) {
                    std::cerr << "Error in request handler thread: " << e.what() << std::endl;
                }
            }}.detach();
        }

        for (auto &thread: thread_pool) {
            thread.join();
        }

    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return "Error: " + std::string(e.what());
    }

    return "Server stopped";
}

void handle_show_command(const std::string &model_name) {
    json data = model_data(model_name);
    std::cout << data.dump(4) << std::endl;
}

void handle_serve_command(const std::string &model_name) {
    std::thread serv_thread(handle_server);
    std::thread run_model_thread(handle_run_command, model_name);
    serv_thread.join();
    run_model_thread.join();
}

void show_commands(int argc, char **argv) {
    // Directory management
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
        } else if (arg == "show") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "Show full information about model" << std::endl;
                    std::cout << std::endl;
                    std::cout << "Usage: " << std::endl;
                    std::cout << "  ./phoenix show MODEL_NAME" << std::endl;
                    return;
                }
                handle_show_command(argv[i + 1]);
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
                    std::cout << "  ./phoenix history [ID]" << std::endl;
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
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "Run local server to interact with llm" << std::endl;
                    std::cout << "Usage:" << std::endl;
                    std::cout << "  ./phoenix serve [model_name]" << std::endl;
                    return;
                }
                handle_serve_command(argv[i + 1]);
                return;
            }

        } else {
            std::cout << "Unknown command. Use --help for usage information." << std::endl;
        }
    }

}

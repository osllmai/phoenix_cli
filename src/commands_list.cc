#include "commands_list.h"
#include "chat_manager.h"
#include "database_manager.h"
#include "directory_manager.h"
#include "download_model.h"
#include "models_list.h"
#include "chat.h"
#include "api/api.h"
#include "web_server.h"
#include "openai.h"


#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <cstdlib>


using json = nlohmann::json;


void print_help() {
    std::cout << "PhoenixCLI is a large language model runner developed by "
                 "osllm.ai. It is open-source software that you can use freely "
                 "to run and manage large language models."
              << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "phoenix [flags]" << std::endl;
    std::cout << "phoenix [command]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  serve\t\tStart the phoenix server" << std::endl;
    std::cout << "  list\t\tDisplays a list of all available models that you can "
                 "run or manage"
              << std::endl;
    std::cout << "  show\t\tShow full information about a model" << std::endl;
    std::cout << "  run\t\tExecutes a model based on your specified input"
              << std::endl;
    std::cout << "  exec\t\tRuns a model directly on your local machine"
              << std::endl;
    std::cout
            << "  pull\t\tRetrieves a model from the registry and stores it locally"
            << std::endl;
    std::cout << "  rm\t\tRemove a model from your local storage" << std::endl;
    std::cout << "  history\tShows a list of your previous chats" << std::endl;
    std::cout << "  help\t\tProvides detailed information and guidance about any "
                 "specific command"
              << std::endl;
    std::cout << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  -h, --help\tDisplays help information for the phoenix command"
              << std::endl;
    std::cout << "  -v, --version\tOutputs the current version of PhoenixCLI"
              << std::endl;
    std::cout << std::endl;
    std::cout << "Use \"phoenix [command] --help\" for more information about a "
                 "command."
              << std::endl;
}

void handle_run_command(const std::string &model_name) {
    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        std::string model_path =
                DatabaseManager::get_path_by_model_name(db, model_name);
        if (model_path.empty()) {
            std::cerr << "Failed to find model: " << model_name << std::endl;
            sqlite3_close(db);
            return;
        }

        json data = model_data(model_name);
        if (data.contains("promptTemplate") && data["promptTemplate"].is_string()) {
            std::string prompt_template = data["promptTemplate"].get<std::string>();
            sqlite3_close(db);
            PhoenixChat::run_command(prompt_template, model_path);
        } else {
            std::cerr
                    << "Error: 'promptTemplate' is missing or not a string for model: "
                    << model_name << std::endl;
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
        std::cerr << "Model not found in the list of available models."
                  << std::endl;
        return;
    }
    std::string model_url = get_url_llm_download(model_name);
    if (model_url.empty()) {
        std::cerr << "Model URL not found." << std::endl;
        return;
    }
    DirectoryManager::create_custom_directory(
            DirectoryManager::get_app_home_path() + "/models",
            model["companyName"].get<std::string>());
    const std::string model_path = DirectoryManager::get_app_home_path() +
                                   "/models/" +
                                   model["companyName"].get<std::string>() + "/" +
                                   model["filename"].get<std::string>();
    if (download_model_file(model_url, model_path)) {
        sqlite3 *db;
        const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
        if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
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
        const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
        if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
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
        std::cout << "You can see which models located in your local machine or "
                     "exists on phoenix to download"
                  << std::endl;
        std::cout << std::endl;
        std::cout << "  --local\tList of local machine LLMs" << std::endl;
        std::cout << "  --remote\tList of phoenix LLMs" << std::endl;
    }
}

void handle_exec_command(const std::string &model_path) {
    std::string prompt_template;
    std::cout << "Enter your prompt template" << std::endl;
    std::cout << "If your leave blank, default template used:\n "
                 "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|"
                 "start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>"
              << std::endl;
    std::cout << ">>> ";

    std::getline(std::cin, prompt_template);

    if (prompt_template.empty()) {
        prompt_template =
                "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_"
                "header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";
    }
    PhoenixChat::run_command(prompt_template, model_path);
}

//void handle_history_command(const std::string &chat_id) {
//    if (chat_id.empty()) {
//        std::cout << "List of your chats:" << std::endl;
//        std::cout << "----------------------------" << std::endl;
//        sqlite3 *db;
//        const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
//        if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
//            DatabaseManager::read_chat_histories(db);
//            sqlite3_close(db);
//        }
//        //        std::vector<std::string> history = ChatManager::chat_histories();
//        //        for (const auto &chat: history) {
//        //            std::cout << chat << std::endl;
//        //        }
//    } else {
//        std::string chat_file_path;
//        sqlite3 *db;
//        const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
//        if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
//            chat_file_path = DatabaseManager::get_path_by_filename(db, chat_id);
//            sqlite3_close(db);
//        }
//        json chat_history = ChatManager::chat_history_conversation(chat_file_path);
//        if (!chat_history.empty()) {
//            std::cout << chat_history.dump(4) << std::endl;
//        } else {
//            std::cout << "No chat history found for ID: " << chat_id << std::endl;
//        }
//    }
//}

void handle_history_command(const std::string &chat_id) {
    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        if (chat_id.empty()) {
            std::cout << "List of your chats:" << std::endl;
            std::cout << "----------------------------" << std::endl;

            std::string select_chats_sql = "SELECT id, created_at FROM chats;";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, select_chats_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    long chat_id = sqlite3_column_int64(stmt, 0);
                    const char *created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                    std::cout << "Chat ID: " << chat_id << ", Created At: " << created_at << std::endl;
                }
                sqlite3_finalize(stmt);
            } else {
                std::cerr << "Error preparing select chats statement: " << sqlite3_errmsg(db) << std::endl;
            }
        } else {
            std::string select_chat_sql = "SELECT id, created_at, prompt, model, temperature FROM chats WHERE id = ?;";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, select_chat_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int64(stmt, 1, std::stol(chat_id));
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    json chat_history;
                    chat_history["chat_id"] = sqlite3_column_int64(stmt, 0);
                    chat_history["created_at"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                    chat_history["prompt"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                    chat_history["model"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                    chat_history["temperature"] = sqlite3_column_double(stmt, 4);

                    std::string select_messages_sql = "SELECT sequence_number, role, content FROM messages WHERE chat_id = ? ORDER BY sequence_number;";
                    sqlite3_stmt *msg_stmt;
                    if (sqlite3_prepare_v2(db, select_messages_sql.c_str(), -1, &msg_stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int64(msg_stmt, 1, std::stol(chat_id));
                        while (sqlite3_step(msg_stmt) == SQLITE_ROW) {
                            json message;
                            message["sequence_number"] = sqlite3_column_int(msg_stmt, 0);
                            message["role"] = reinterpret_cast<const char *>(sqlite3_column_text(msg_stmt, 1));
                            message["content"] = reinterpret_cast<const char *>(sqlite3_column_text(msg_stmt, 2));
                            chat_history["messages"].push_back(message);
                        }
                        sqlite3_finalize(msg_stmt);
                    } else {
                        std::cerr << "Error preparing select messages statement: " << sqlite3_errmsg(db) << std::endl;
                    }

                    std::cout << chat_history.dump(4) << std::endl;
                } else {
                    std::cout << "No chat history found for ID: " << chat_id << std::endl;
                }
                sqlite3_finalize(stmt);
            } else {
                std::cerr << "Error preparing select chat statement: " << sqlite3_errmsg(db) << std::endl;
            }
        }
        sqlite3_close(db);
    } else {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    }
}

void handle_rm_command(const std::string &model_name) {
    if (DirectoryManager::delete_model(model_name)) {
        std::cout << "Model deleted" << std::endl;
        sqlite3 *db;
        const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
        if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
            DatabaseManager::delete_model(db, model_name);
            sqlite3_close(db);
        }
    } else {
        std::cout << "Model not found" << std::endl;
    }
}

void handle_show_command(const std::string &model_name) {
    json data = model_data(model_name);
    std::cout << data.dump(4) << std::endl;
}

void handle_serve_command(const std::string &model_name) {
    std::thread serv_thread(handle_server);
    std::thread run_model_thread(handle_run_command, model_name);
    std::thread endpoint_threads(endpoints);
    serv_thread.join();
    run_model_thread.join();
    endpoint_threads.join();
}

void handle_openai_command() {
    try {
        // Read the API key from the environment variable
        const char *api_key_env = std::getenv("OPENAI_API_KEY");
        if (!api_key_env) {
            throw std::runtime_error("OPENAI_API_KEY environment variable not set");
        }
        std::string api_key = api_key_env;

        OpenAI openai(api_key);

        std::string prompt;
        std::cout << "Enter a prompt: " << std::endl;
        std::getline(std::cin, prompt);

        // Prepare the messages for the chat completion
        json messages = json::array({
                                            {{"role", "system"}, {"content", "You are a helpful assistant."}},
                                            {{"role", "user"},   {"content", prompt}}
                                    });

        // Define a callback function to handle the streamed response
        auto callback = [](const std::string &content) {
            std::cout << content << std::flush;
            return true;  // Return true to continue processing
        };

        // Stream the chat completion
        std::atomic<bool> connection_alive(true);
        std::cout << "AI response:" << std::endl;
        openai.stream_chat_completion("gpt-3.5-turbo", messages, callback, connection_alive);
        std::cout << std::endl << "Story complete." << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void show_commands(int argc, char **argv) {
    // Directory management
    DirectoryManager::handle_application_directory();
    DirectoryManager::create_custom_directory(
            DirectoryManager::get_app_home_path(), "models");
    DirectoryManager::create_custom_directory(
            DirectoryManager::get_app_home_path(), "chats");

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "help" || arg == "--help") {
            print_help();
            return;
        } else if (arg == "run") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help") ||
                    argv[i + 1] == std::string("-h")) {
                    std::cout << "Run a model" << std::endl;
                    return;
                }
                handle_run_command(argv[i + 1]);
                return;
            } else {
                std::cout << "Error: 'run' command requires a model argument"
                          << std::endl;
                return;
            }
        } else if (arg == "pull") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help") ||
                    argv[i + 1] == std::string("-h")) {
                    std::cout << "Download LLM" << std::endl;
                    return;
                }
                handle_pull_command(argv[i + 1]);
                return;
            }
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "Current version is " << VERSION << std::endl;
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
                    std::cout << "All chat conversations generated in your home"
                              << std::endl;
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

        } else if (arg == "openai") {
            if (i + 1 < argc) {
                if (argv[i + 1] == std::string("--help")) {
                    std::cout << "Interact with OpenAI" << std::endl;
                    std::cout << "Usage:" << std::endl;
                    std::cout << "  ./phoenix openai [api_key]" << std::endl;
                    return;
                }
                return;
            }
            handle_openai_command();
            return;
        } else {
            std::cout << "Unknown command. Use --help for usage information."
                      << std::endl;
        }
    }
}



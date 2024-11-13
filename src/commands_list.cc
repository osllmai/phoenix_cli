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
#include "llama.h"
#include "embedding.h"
#include "server/server.h"


#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <cstdlib>

PhoenixCommandsList::PhoenixCommandsList() {
    commands_["help"] = [this](const auto &args) { print_help(); };
    commands_["server"] = [this](const auto &args) { start_server(args); };
    commands_["embedding"] = [this](const auto &args) { handle_embedding(args); };
    commands_["openai"] = [this](const auto &args) { handle_openai(args); };
    commands_["list"] = [this](const auto &args) { handle_list(args); };
    commands_["run"] = [this](const auto &args) { handle_run(args); };
    commands_["pull"] = [this](const auto &args) { handle_pull(args); };
    commands_["rm"] = [this](const auto &args) { handle_remove(args); };
    commands_["history"] = [this](const auto &args) { handle_history(args); };
    commands_["exec"] = [this](const auto &args) { handle_exec(args); };
    commands_["show"] = [this](const auto &args) { handle_show(args); };

}

void PhoenixCommandsList::print_help() {
    std::string help_text = R"(
PhoenixCLI is a large language model runner developed by osllm.ai. It is open-source software that you can use freely
to run and manage large language models.
Usage:
phoenix_cli [flags]
phoenix_cli [command]

Available Commands:
  server        Start the phoenix_cli server
  list          Displays a list of all available models that you can run or manage
  show          Show full information about a model
  run           Executes a model based on your specified input
  embedding     Convert text to embedding
  exec          Runs a model directly on your local machine
  pull          Retrieves a model from the registry and stores it locally
  rm            Remove a model from your local storage
  help          Provides detailed information and guidance about any specific command
  openai        Interact with OpenAI models

Flags:
  -h, --help    Displays help information for the phoenix_cli command
  -v, --version Outputs the current version of PhoenixCLI

Use "phoenix_cli [command] --help" for more information about a command.
)";
    std::cout << help_text;
}


void PhoenixCommandsList::handle_run(const std::vector<std::string> &args) {
    try {
        const std::string &model_name = args[0];
        auto db = DatabaseManager::open_database();
        std::string model_path = DatabaseManager::get_path_by_model_name(db.get(), model_name);

        if (model_path.empty()) {
            throw std::runtime_error("Failed to find model: " + model_name);
        }

        PhoenixCommandsList::json data = PhoenixModelList::model_data(model_name);

        if (!data.contains("promptTemplate") || !data["promptTemplate"].is_string()) {
            throw std::runtime_error("Error: 'promptTemplate' is missing or not a string for model: " + model_name);
        }

        auto resources = PhoenixChat::load_model(model_path);

        PhoenixChat::handle_conversation(*resources, true);

    } catch (const std::exception &e) {
        std::cerr << "Fatal error in run command: " << e.what() << std::endl;
    }
}

void PhoenixCommandsList::handle_pull(const std::vector<std::string> &args) {
    const std::string &model_name = args[0];
    PhoenixCommandsList::json model = PhoenixModelList::model_data(model_name);
    std::string model_url = PhoenixModelList::get_url_llm_download(model_name);

    DirectoryManager::create_custom_directory(
            DirectoryManager::get_app_home_path() + "/models",
            model["companyName"].get<std::string>());

    const std::string model_path = DirectoryManager::get_app_home_path() +
                                   "/models/" +
                                   model["companyName"].get<std::string>() + "/" +
                                   model["filename"].get<std::string>();

    PhoenixDownloadModel downloader(model_url, model_path);
    if (downloader.download()) {
        auto db = DatabaseManager::open_database();
        DatabaseManager::insert_models(db.get(), model_name, model_path);
        std::cout << "Model downloaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to complete download model" << std::endl;
    }
}

void PhoenixCommandsList::handle_list(const std::vector<std::string> &args) {
    const std::string &option = args[0];

    if (option == "--local") {
        std::cout << R"(
List models which downloaded
----------------------------\n
)";
        auto db = DatabaseManager::open_database();
        DatabaseManager::read_models(db.get());

    } else if (option == "--remote") {
        std::cout << R"(
List models which you can download
----------------------------
)";
        PhoenixCommandsList::json list_of_models = PhoenixModelList::list_of_models_available();
        for (const auto &[key, value]: list_of_models.items()) {
            std::cout << key << std::endl;
        }
    } else {
        std::cout << R"(
You can see which models located in your local machine or exists on phoenix to download

 --local\tList of local machine LLMs
 --remote\tList of phoenix LLMs
        )" << std::endl;
    }
}


void PhoenixCommandsList::handle_exec(const std::vector<std::string> &args) {
    auto recources = PhoenixChat::load_model(args[0]);
    PhoenixChat::handle_conversation(*recources, true);
}

bool execute_query(sqlite3 *db, const std::string &sql, sqlite3_stmt **stmt) {
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr) == SQLITE_OK) {
        return true;
    } else {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
}

void fetch_chat_history(sqlite3 *db, const std::string &chat_id) {
    std::string select_chat_sql = "SELECT id, created_at, prompt, model, temperature FROM chats WHERE id = ?;";
    sqlite3_stmt *stmt;
    if (execute_query(db, select_chat_sql, &stmt)) {
        sqlite3_bind_int64(stmt, 1, std::stol(chat_id));
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            PhoenixCommandsList::json chat_history;
            chat_history["chat_id"] = sqlite3_column_int64(stmt, 0);
            chat_history["created_at"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            chat_history["prompt"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            chat_history["model"] = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            chat_history["temperature"] = sqlite3_column_double(stmt, 4);

            std::string select_messages_sql = "SELECT sequence_number, role, content FROM messages WHERE chat_id = ? ORDER BY sequence_number;";
            sqlite3_stmt *msg_stmt;
            if (execute_query(db, select_messages_sql, &msg_stmt)) {
                sqlite3_bind_int64(msg_stmt, 1, std::stol(chat_id));
                while (sqlite3_step(msg_stmt) == SQLITE_ROW) {
                    PhoenixCommandsList::json message;
                    message["sequence_number"] = sqlite3_column_int(msg_stmt, 0);
                    message["role"] = reinterpret_cast<const char *>(sqlite3_column_text(msg_stmt, 1));
                    message["content"] = reinterpret_cast<const char *>(sqlite3_column_text(msg_stmt, 2));
                    chat_history["messages"].push_back(message);
                }
                sqlite3_finalize(msg_stmt);
            }

            std::cout << chat_history.dump(4) << std::endl;
        } else {
            std::cout << "No chat history found for ID: " << chat_id << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}


void PhoenixCommandsList::handle_history(const std::vector<std::string> &args) {
    auto db = DatabaseManager::open_database();

    if (args.size() == 0) {
        std::cout << "List of your chats:" << std::endl;
        std::cout << "----------------------------" << std::endl;

        std::string select_chats_sql = "SELECT id, created_at FROM chats;";
        sqlite3_stmt *stmt;
        if (execute_query(db.get(), select_chats_sql, &stmt)) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                long chat_id = sqlite3_column_int64(stmt, 0);
                const char *created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                std::cout << "Chat ID: " << chat_id << ", Created At: " << created_at << "\n";
            }
            sqlite3_finalize(stmt);
        }
    } else {
        fetch_chat_history(db.get(), args[0]);
    }
}

void PhoenixCommandsList::handle_remove(const std::vector<std::string> &args) {
    const std::string &model_name = args[0];
    if (DirectoryManager::delete_model(model_name)) {
        std::cout << "Model deleted" << std::endl;
        auto db = DatabaseManager::open_database();
        DatabaseManager::delete_model(db.get(), model_name);
    } else {
        std::cout << "Model not found" << std::endl;
    }
}

void PhoenixCommandsList::handle_show(const std::vector<std::string> &args) {
    PhoenixCommandsList::json data = PhoenixModelList::model_data(args[0]);
    std::cout << data.dump(4) << std::endl;
}

//
//void handle_serve_command(const std::string &model_name) {
//    try {
//        std::thread serv_thread(handle_server);
//        std::thread run_model_thread(handle_run_command, model_name);
//        std::thread endpoint_threads(endpoints);
//
//        // Join the threads to prevent the main process from exiting
//        serv_thread.join();
//        run_model_thread.join();
//        endpoint_threads.join();
//    } catch (const std::exception &e) {
//        std::cerr << "Error starting serve command: " << e.what() << std::endl;
//    } catch (...) {
//        std::cerr << "Unknown error starting serve command." << std::endl;
//    }
//}
//
//
void PhoenixCommandsList::handle_openai(const std::vector<std::string> &args) {
    std::string model_name = args.size() == 0 ? "gpt-4o-mini" : args[0];
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

        if (prompt.empty()) {
            throw std::runtime_error("Prompt cannot be empty");
        }

        // Prepare the messages for the chat completion
        PhoenixCommandsList::json messages = PhoenixCommandsList::json::array({
                                                                                      {{"role", "system"}, {"content", "You are a helpful assistant."}},
                                                                                      {{"role", "user"},   {"content", prompt}}
                                                                              });

        // Define a callback function to handle the streamed response
        auto callback = [](const std::string &content) {
            std::cout << content << std::flush;
            return true;
        };

        // Stream the chat completion
        std::atomic<bool> connection_alive(true);

        std::cout << "AI response:" << std::endl;
        openai.stream_chat_completion(model_name, messages, callback, connection_alive);

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void PhoenixCommandsList::start_server(const std::vector<std::string> &args) {
    std::vector<std::string> filtered_args;

    for (int i = 1; i < args.size(); ++i) {
        filtered_args.push_back(args[i]);
    }

    std::vector<char *> filtered_argv;
    for (const auto &str: filtered_args) {
        filtered_argv.push_back(const_cast<char *>(str.c_str()));
    }

    LLamaWebServer::llama_server(filtered_argv.size(), filtered_argv.data());
}


void PhoenixCommandsList::handle_embedding(const std::vector<std::string> &args) {
    std::vector<std::string> filtered_args;
    for (int i = 1; i < args.size(); ++i) {
        filtered_args.push_back(args[i]);
    }

    std::vector<char *> filtered_argv;
    for (const auto &str: filtered_args) {
        filtered_argv.push_back(const_cast<char *>(str.c_str()));
    }

    PhoenixEmbedding::embedding(filtered_argv.size(), filtered_argv.data());
}

void PhoenixCommandsList::run_command(const std::string &command, const std::vector<std::string> &args) {
    auto it = commands_.find(command);
    if (it != commands_.end()) {
        it->second(args);
    } else {
        std::cerr << "Error: Command '" << command << "' not recognized. Type 'help' for a list of available commands."
                  << std::endl;
    }
}
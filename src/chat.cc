#include "header.h"
#include "llmodel.h"
#include "utils.h"
#include "parse_json.h"
#include "chat_manager.h"
#include "database_manager.h"
#include "directory_manager.h"

#include <string>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

std::atomic<bool> stop_display{false};
std::string answer = "";
LLModel *global_model = nullptr;

// Function to display animation frames
void display_frames() {
    const char *frames[] = {".", ":", "'", ":"};
    int frame_index = 0;
    ConsoleState con_st;
    con_st.use_color = true;
    while (!stop_display) {
        set_console_color(con_st, PROMPT);
        std::cerr << "\r" << frames[frame_index % 4] << std::flush;
        frame_index++;
        set_console_color(con_st, DEFAULT);
        if (!stop_display) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cerr << "\r" << " " << std::flush;
            std::cerr << "\r" << std::flush;
        }
    }
}

// Function to display loading animation
void display_loading() {
    while (!stop_display) {
        for (int i = 0; i < 14; i++) {
            fprintf(stdout, ".");
            fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (stop_display) { break; }
        }
        std::cout << "\r" << "               " << "\r" << std::flush;
    }
    std::cout << "\r" << " " << std::flush;
}

// Function to get user input
std::string get_input(ConsoleState &con_st, std::string &input, chatParams &params) {
    set_console_color(con_st, USER_INPUT);
    std::cout << "\n> ";
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::string input1, input2;
    std::getline(iss, input1, ' ');
    std::getline(iss, input2, ' ');
    set_console_color(con_st, DEFAULT);
    return input;
}

// Function to initialize the model
LLModel *
initialize_model(const std::string &model_path, int n_ctx = 4096, int ngl = 100, const std::string &backend = "auto") {
    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = n_ctx;

    LLModel *model = LLModel::Implementation::construct(model_path, backend, prompt_context.n_ctx);

#if defined(WIN32)
    backend = "cuda";
    if (backend == "cuda") {
        auto devices = LLModel::Implementation::availableGPUDevices();
        if (!devices.empty()) {
            for (const auto& device : devices) {
                //std::cout << "Found GPU: " << device.selectionName() << " with heap size: " << device.heapSize << std::endl;
            }
            // Example: Initialize the first available device
            size_t memoryRequired = devices[0].heapSize;
            const std::string& name = devices[0].name;
            const size_t requiredMemory = model->requiredMem(model_path, prompt_context.n_ctx, ngl);
            auto devices = model->availableGPUDevices(memoryRequired);
            for (const auto& device : devices) {
                if (device.name == name && model->initializeGPUDevice(device.index)) {
                    break;
                }
            }
        }
    }
#endif

    std::future<void> future;
    stop_display = false;
    future = std::async(std::launch::async, display_loading);

    auto check_model = model->loadModel(model_path, prompt_context.n_ctx, ngl);
    stop_display = true;
    future.wait();

    if (!check_model) {
        std::cerr << "Error loading: " << model_path << std::endl;
        delete model;
        return nullptr;
    }

    std::cout << "\r" << APPNAME << ": done loading!" << std::flush;
    model->setThreadCount(8);
    return model;
}

// Function to handle a single conversation
std::string handle_conversation(LLModel *model, const std::string &prompt_template, const std::string &prompt,
                                const chatParams &params, bool is_api_call = false,
                                std::function<void(const std::string &)> token_callback = nullptr) {
    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 4096;

    auto prompt_callback = [](int32_t token_id) { return true; };

    auto response_callback = [&token_callback, is_api_call](int32_t token_id, const std::string &responsechars_str) {
        if (!responsechars_str.empty()) {
            if (is_api_call && token_callback) {
                token_callback(responsechars_str);
            } else {
                std::cout << responsechars_str << std::flush;
                answer += responsechars_str;
            }
        }
        return true;
    };

    auto recalculate_callback = [](bool is_recalculating) { return is_recalculating; };

    std::string unique_identifier = ChatManager::generate_unique_id();
    ChatManager::create_chat_config_file(unique_identifier, params);

    if (!params.b_token.empty()) {
        answer += params.b_token;
        if (!is_api_call) { std::cout << params.b_token; }
    }

    model->prompt(prompt, prompt_template, prompt_callback, response_callback, recalculate_callback, prompt_context,
                  false, nullptr);

    if (!params.e_token.empty()) {
        if (!is_api_call) { std::cout << params.e_token; }
        answer += params.e_token;
    }

    if (!params.save_log.empty()) {
        save_chat_log(params.save_log, params.default_prefix + params.default_header + prompt + params.default_footer,
                      answer);
    }

    std::string path = ChatManager::save_chat_history(unique_identifier, prompt, answer);

    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        DatabaseManager::insert_chat_history(db, unique_identifier, path);
        sqlite3_close(db);
    } else {
        std::cerr << "error in open db" << std::endl;
    }

    std::string response = answer;
    answer.clear();
    return response;
}

// Function to handle a conversation with chat history
std::string handle_conversation(LLModel *model, const std::string &prompt_template,
                                const std::vector<std::pair<std::string, std::string>> &chat_history,
                                const chatParams &params, bool is_api_call = false,
                                std::function<void(const std::string &)> token_callback = nullptr) {
    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 4096;

    // Convert chat history to a format the model can understand
    std::string combined_prompt = prompt_template;
    for (const auto &entry: chat_history) {
        combined_prompt += "User: " + entry.second + "\n";
        combined_prompt += "Assistant: " + entry.first + "\n";
    }

    auto prompt_callback = [](int32_t token_id) { return true; };

    auto response_callback = [&token_callback, is_api_call](int32_t token_id, const std::string &responsechars_str) {
        if (!responsechars_str.empty()) {
            if (is_api_call && token_callback) {
                token_callback(responsechars_str);
            } else {
                std::cout << responsechars_str << std::flush;
                answer += responsechars_str;
            }
        }
        return true;
    };

    auto recalculate_callback = [](bool is_recalculating) { return is_recalculating; };

    std::string unique_identifier = ChatManager::generate_unique_id();
    ChatManager::create_chat_config_file(unique_identifier, params);

    if (!params.b_token.empty()) {
        answer += params.b_token;
        if (!is_api_call) { std::cout << params.b_token; }
    }

    model->prompt(combined_prompt, prompt_template, prompt_callback, response_callback, recalculate_callback,
                  prompt_context, false, nullptr);

    if (!params.e_token.empty()) {
        if (!is_api_call) { std::cout << params.e_token; }
        answer += params.e_token;
    }

    if (!params.save_log.empty()) {
        save_chat_log(params.save_log,
                      params.default_prefix + params.default_header + combined_prompt + params.default_footer, answer);
    }

    std::string path = ChatManager::save_chat_history(unique_identifier, combined_prompt, answer);

    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        DatabaseManager::insert_chat_history(db, unique_identifier, path);
        sqlite3_close(db);
    } else {
        std::cerr << "error in open db" << std::endl;
    }

    std::string response = answer;
    answer.clear();
    return response;
}

// Function to run the command-line interface
int run_command(const std::string &prompt_template, const std::string &model_path) {
    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << APPNAME;
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")\n\n";

    chatParams params;
    params.model = model_path;

    if (!global_model) {
        global_model = initialize_model(model_path);
        if (!global_model) {
            std::cout << "Press any key to exit..." << std::endl;
            std::cin.get();
            return 0;
        }
    }

    std::string input = "";
    while (true) {
        input = get_input(con_st, input, params);
        handle_conversation(global_model, prompt_template, input, params);
    }

    return 0;
}

// Function to chat with the API
std::string
chat_with_api(const std::string &prompt_template, const std::string &model_path, const std::string &prompt) {
    chatParams params;
    params.model = model_path;

    if (!global_model) {
        global_model = initialize_model(model_path);
        if (!global_model) {
            return "Error: Failed to initialize model.";
        }
    }

    return handle_conversation(global_model, prompt_template, prompt, params, true);
}

// Function to chat with the API and stream the response
std::string
chat_with_api_stream(const std::string &prompt_template, const std::string &model_path, const std::string &prompt,
                     std::function<void(const std::string &)> token_callback) {
    chatParams params;
    params.model = model_path;

    if (!global_model) {
        global_model = initialize_model(model_path);
        if (!global_model) {
            return "Error: Failed to initialize model.";
        }
    }

    return handle_conversation(global_model, prompt_template, prompt, params, true, token_callback);
}

// Function to chat with the API using chat history
std::string chat_with_api(const std::string &prompt_template, const std::string &model_path,
                          const std::vector<std::pair<std::string, std::string>> &chat_history) {
    chatParams params;
    params.model = model_path;

    if (!global_model) {
        global_model = initialize_model(model_path);
        if (!global_model) {
            return "Error: Failed to initialize model.";
        }
    }

    return handle_conversation(global_model, prompt_template, chat_history, params, true);
}

// Function to chat with the API and stream the response using chat history
std::string chat_with_api_stream(const std::string &prompt_template, const std::string &model_path,
                                 const std::vector<std::pair<std::string, std::string>> &chat_history,
                                 std::function<void(const std::string &)> token_callback) {
    chatParams params;
    params.model = model_path;

    if (!global_model) {
        global_model = initialize_model(model_path);
        if (!global_model) {
            return "Error: Failed to initialize model.";
        }
    }

    return handle_conversation(global_model, prompt_template, chat_history, params, true, token_callback);
}
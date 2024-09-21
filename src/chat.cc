#include "chat.h"
#include "header.h"
#include "llmodel.h"
#include "utils.h"
#include "parse_json.h"
#include "chat_manager.h"
#include "database_manager.h"
#include "directory_manager.h"
#include "api/include/utils/utils.h"

#include <string>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>
#include <sqlite3.h>
#include <functional>


std::atomic<bool> stop_display{false};
std::string answer = "";
LLModel *global_model = nullptr;

// Function to display animation frames
void PhoenixChat::display_frames() {
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
void PhoenixChat::display_loading() {
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
std::string PhoenixChat::get_input(ConsoleState &con_st, std::string &input, chatParams &params) {
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
PhoenixChat::initialize_model(const std::string &model_path, int n_ctx, int ngl, const std::string &backend) {
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
std::string PhoenixChat::handle_conversation(LLModel *model, const std::string &prompt_template, const std::string &prompt,
                                             const chatParams &params, bool is_api_call,
                                             std::function<void(const std::string &)> token_callback) {
    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 4096;

    auto prompt_callback = [](int32_t token_id) { return true; };

    auto response_callback = [&token_callback, is_api_call](int32_t token_id, const std::string &responsechars_str) {
        if (!responsechars_str.empty()) {
            if (is_api_call && token_callback) {
                answer += responsechars_str;
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
        // Insert chat entry with only non-null columns
        std::string insert_chat_sql = "INSERT INTO chats (id, created_at, sharing, context_length, description, embeddings_provider, include_profile_context, include_workspace_instructions, model, name, prompt, temperature) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt *stmt;
        std::string current_time = get_current_time();
        if (sqlite3_prepare_v2(db, insert_chat_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier));
            sqlite3_bind_text(stmt, 2, current_time.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, params.n_ctx);
            sqlite3_bind_text(stmt, 4, "", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, "", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 6, 0);
            sqlite3_bind_int(stmt, 7, 0);
            sqlite3_bind_text(stmt, 8, "", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 9, "", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 10, prompt.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 11, params.temp);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error inserting chat: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Error preparing chat insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        // Insert user message entry
        std::string insert_user_message_sql = "INSERT INTO messages (chat_id, created_at, content, role, sequence_number) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, insert_user_message_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier)); // Assuming unique_identifier is the chat_id
            sqlite3_bind_text(stmt, 2, prompt.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, "user", -1, SQLITE_STATIC); // role is "user"
            sqlite3_bind_int(stmt, 4, 1); // sequence_number

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error inserting user message: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Error preparing user message insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        // Insert assistant message entry
        std::string insert_assistant_message_sql = "INSERT INTO messages (chat_id, created_at, content, role, sequence_number) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, insert_assistant_message_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier)); // Assuming unique_identifier is the chat_id
            sqlite3_bind_text(stmt, 2, answer.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, "assistant", -1, SQLITE_STATIC); // role is "assistant"
            sqlite3_bind_int(stmt, 4, 2); // sequence_number

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error inserting assistant message: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Error preparing assistant message insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_close(db);
    } else {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    }

    std::string response = answer;
    answer.clear();
    return response;
}

// Function to handle a conversation with chat history
std::string PhoenixChat::handle_conversation(LLModel *model, const std::string &prompt_template,
                                             const std::vector<std::pair<std::string, std::string>> &chat_history,
                                             const chatParams &params, bool is_api_call,
                                             std::function<void(const std::string &)> token_callback) {
    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 4096;

    // Convert chat history to a format the model can understand
    std::string combined_prompt = prompt_template;
    for (const auto &entry : chat_history) {
        combined_prompt += entry.first + ": " + entry.second + "\n";
    }

    auto prompt_callback = [](int32_t token_id) { return true; };

    auto response_callback = [&token_callback, is_api_call](int32_t token_id, const std::string &responsechars_str) {
        if (!responsechars_str.empty()) {
            if (is_api_call && token_callback) {
                answer += responsechars_str;
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
        // Insert chat entry with only non-null columns
        std::string insert_chat_sql = "INSERT INTO chats (id, created_at, sharing, context_length, description, embeddings_provider, include_profile_context, include_workspace_instructions, model, name, prompt, temperature) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt *stmt;
        std::string current_time = get_current_time();
        if (sqlite3_prepare_v2(db, insert_chat_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier));
            sqlite3_bind_text(stmt, 2, "", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, params.n_ctx);
            sqlite3_bind_text(stmt, 5, "CLI", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, "", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 6, 0);
            sqlite3_bind_int(stmt, 7, 0);
            sqlite3_bind_text(stmt, 8, params.model.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 9, "", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 10, combined_prompt.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 11, params.temp);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error inserting chat: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Error preparing chat insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        // Insert user and assistant message entries from chat history
        int sequence_number = 1;
        for (const auto &entry : chat_history) {
            std::string insert_message_sql = "INSERT INTO messages (chat_id, created_at, content, role, sequence_number) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?);";
            if (sqlite3_prepare_v2(db, insert_message_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier)); // Assuming unique_identifier is the chat_id
                sqlite3_bind_text(stmt, 2, entry.second.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, entry.first.c_str(), -1, SQLITE_STATIC); // role is either "user" or "assistant"
                sqlite3_bind_int(stmt, 4, sequence_number++); // sequence_number

                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    std::cerr << "Error inserting message: " << sqlite3_errmsg(db) << std::endl;
                }
                sqlite3_finalize(stmt);
            } else {
                std::cerr << "Error preparing message insert statement: " << sqlite3_errmsg(db) << std::endl;
            }
        }

        // Insert the final assistant response message
        std::string insert_final_assistant_message_sql = "INSERT INTO messages (chat_id, created_at, content, role, sequence_number) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, insert_final_assistant_message_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier)); // Assuming unique_identifier is the chat_id
            sqlite3_bind_text(stmt, 2, answer.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, "assistant", -1, SQLITE_STATIC); // role is "assistant"
            sqlite3_bind_int(stmt, 4, sequence_number); // sequence_number

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error inserting final assistant message: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Error preparing final assistant message insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_close(db);
    } else {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    }

    std::string response = answer;
    answer.clear();
    return response;
}

// Function to run the command-line interface
int PhoenixChat::run_command(const std::string &prompt_template, const std::string &model_path) {
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
PhoenixChat::chat_with_api(const std::string &prompt_template, const std::string &model_path, const std::string &prompt) {
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
PhoenixChat::chat_with_api_stream(const std::string &prompt_template, const std::string &model_path, const std::string &prompt,
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
std::string PhoenixChat::chat_with_api(const std::string &prompt_template, const std::string &model_path,
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
std::string PhoenixChat::chat_with_api_stream(const std::string &prompt_template, const std::string &model_path,
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
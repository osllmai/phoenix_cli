#include "llama.h"

#include "chat.h"
#include "header.h"
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
#include <stdexcept>
#include <vector>
#include <functional>


std::unique_ptr<PhoenixChat::LlamaResources> PhoenixChat::load_model(
        const std::string &model_path, int n_ctx, int ngl) {

    auto resources = std::make_unique<LlamaResources>();

    llama_log_set([](enum ggml_log_level level, const char *text, void *) {
        if (level >= GGML_LOG_LEVEL_ERROR) {
            fprintf(stderr, "%s", text);
        }
    }, nullptr);

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = ngl;

    resources->model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!resources->model) {
        throw std::runtime_error("Unable to load model");
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;
    ctx_params.n_batch = n_ctx;


    resources->ctx = llama_new_context_with_model(resources->model, ctx_params);
    if (!resources->ctx) {
        llama_free_model(resources->model);
        throw std::runtime_error("Failed to create llama_context");
    }

    resources->smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    if (!resources->smpl) {
        llama_free(resources->ctx);
        llama_free_model(resources->model);
        throw std::runtime_error("Failed to create sampler");
    }

    llama_sampler_chain_add(resources->smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(resources->smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(resources->smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    return resources;
}

void PhoenixChat::generate_response(
        PhoenixChat::LlamaResources &resources, const std::string &prompt,
        std::function<void(const std::string &)> token_callback) {
    llama_model *model = resources.model;
    llama_context *ctx = resources.ctx;
    llama_sampler *smpl = resources.smpl;

    const int n_prompt_tokens = -llama_tokenize(model, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (llama_tokenize(model, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(),
                       llama_get_kv_cache_used_cells(ctx) == 0, true) < 0) {
        GGML_ABORT("failed to tokenize the prompt\n");
    }

    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
    llama_token new_token_id;

    while (true) {
        int n_ctx_used = llama_get_kv_cache_used_cells(ctx);
        if (n_ctx_used + batch.n_tokens > llama_n_ctx(ctx)) {
            fprintf(stderr, "context size exceeded\n");
            return;
        }

        if (llama_decode(ctx, batch)) {
            GGML_ABORT("failed to decode\n");
        }

        new_token_id = llama_sampler_sample(smpl, ctx, -1);
        if (llama_token_is_eog(model, new_token_id)) {
            break;
        }

        char buf[256];
        int n = llama_token_to_piece(model, new_token_id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            GGML_ABORT("failed to convert token to piece\n");
        }

        std::string piece(buf, n);

        // Call the token callback with the current token
        token_callback(piece);

        batch = llama_batch_get_one(&new_token_id, 1);
    }
}


void PhoenixChat::handle_conversation(PhoenixChat::LlamaResources &resources, bool is_stream) {
    std::vector<llama_chat_message> messages;
    std::vector<char> formatted(llama_n_ctx(resources.ctx));
    int prev_len = 0;
    std::string unique_identifier = PhornixChatManager::generate_unique_id();
    PhornixChatManager::create_chat_config_file(unique_identifier);

    while (true) {
        printf("\033[32m> \033[0m");
        std::string user_input;
        std::getline(std::cin, user_input);

        if (user_input.empty())
            break;


        messages.push_back({"user", strdup(user_input.c_str())});
        int new_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), true,
                                                formatted.data(), formatted.size());
        if (new_len > static_cast<int>(formatted.size())) {
            formatted.resize(new_len);
            new_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), true,
                                                formatted.data(), formatted.size());
        }
        if (new_len < 0) {
            fprintf(stderr, "failed to apply the chat template\n");
            return;
        }
        std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);

        std::string full_response = "";
        if (is_stream) {
            PhoenixChat::generate_response(resources, prompt, [&](const std::string &token) {
                std::cout << token << std::flush;
                full_response += token;
            });
        } else {
            PhoenixChat::generate_response(resources, prompt, [&](const std::string &token) {
                full_response += token;
            });
            std::cout << full_response.c_str() << std::endl;
        }

        printf("\n\033[0m");

        messages.push_back({"assistant", strdup(full_response.c_str())});
        prev_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), false, nullptr,
                                             0);
        if (prev_len < 0) {
            fprintf(stderr, "failed to apply the chat template\n");
            return;
        }

        std::string path = PhornixChatManager::save_chat_history(unique_identifier, user_input, full_response);

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
                sqlite3_bind_int(stmt, 3, 2048);
                sqlite3_bind_text(stmt, 4, "", -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 5, "", -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 6, 0);
                sqlite3_bind_int(stmt, 7, 0);
                sqlite3_bind_text(stmt, 8, "", -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 9, "", -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 10, prompt.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_double(stmt, 11, 0.5);

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
                sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier));
                sqlite3_bind_text(stmt, 2, user_input.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, "user", -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 4, 1);

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
                sqlite3_bind_int64(stmt, 1, std::stol(unique_identifier));
                sqlite3_bind_text(stmt, 2, full_response.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, "assistant", -1, SQLITE_STATIC);
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
    }

    for (auto &msg: messages) {
        free(const_cast<char *>(msg.content));
    }
}

std::string PhoenixChat::handle_conversation_with_api(const std::string &prompt, PhoenixChat::LlamaResources &resources,
                                                      bool is_stream,
                                                      std::function<bool(const std::string &)> token_callback) {
    std::vector<llama_chat_message> messages;
    std::vector<char> formatted(llama_n_ctx(resources.ctx));
    int prev_len = 0;

    printf("\033[32m> \033[0m");

    if (prompt.empty())
        return "";

    // Allocate memory for the user prompt
    char *user_prompt_dup = strdup(prompt.c_str());
    if (!user_prompt_dup) {
        fprintf(stderr, "Memory allocation failed\n");
        return "";
    }

    messages.push_back({"user", user_prompt_dup});
    int new_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), true,
                                            formatted.data(), formatted.size());
    if (new_len > static_cast<int>(formatted.size())) {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), true,
                                            formatted.data(), formatted.size());
    }
    if (new_len < 0) {
        fprintf(stderr, "failed to apply the chat template\n");
        free(user_prompt_dup); // Free the allocated memory before returning
        return "";
    }
    std::string user_prompt(formatted.begin() + prev_len, formatted.begin() + new_len);

    std::string full_response = "";
    if (is_stream) {
        PhoenixChat::generate_response(resources, user_prompt, [&](const std::string &token) {
            if (!token_callback(token)) {
                return false;
            }
            return true;
        });
    } else {
        PhoenixChat::generate_response(resources, user_prompt, [&](const std::string &token) {
            full_response += token;
            return true;
        });
    }

    printf("\n\033[0m");

    // Allocate memory for the assistant response
    char *assistant_response_dup = strdup(full_response.c_str());
    if (!assistant_response_dup) {
        fprintf(stderr, "Memory allocation failed\n");
        free(user_prompt_dup); // Free the allocated memory before returning
        return "";
    }

    messages.push_back({"assistant", assistant_response_dup});
    prev_len = llama_chat_apply_template(resources.model, nullptr, messages.data(), messages.size(), false, nullptr,
                                         0);
    if (prev_len < 0) {
        fprintf(stderr, "failed to apply the chat template\n");
        free(user_prompt_dup); // Free the allocated memory before returning
        free(assistant_response_dup); // Free the allocated memory before returning
        return "";
    }

    // Free the allocated memory
    free(user_prompt_dup);
    free(assistant_response_dup);

    return full_response;
}

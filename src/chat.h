#pragma once

#include "header.h"
#include "llama.h"

#include "utils.h"

#include <string>
#include <memory>
#include <functional>


namespace PhoenixChat {

    struct LlamaResources {
        llama_model* model;
        llama_context* ctx;
        llama_sampler* smpl;

        // Clean up resources
        ~LlamaResources() {
            if (smpl) llama_sampler_free(smpl);
            if (ctx) llama_free(ctx);
            if (model) llama_free_model(model);
        }

        // Prevent copying
        LlamaResources(const LlamaResources&) = delete;
        LlamaResources& operator=(const LlamaResources&) = delete;

        // Allow moving
        LlamaResources(LlamaResources&& other) noexcept :
                model(other.model), ctx(other.ctx), smpl(other.smpl) {
            other.model = nullptr;
            other.ctx = nullptr;
            other.smpl = nullptr;
        }

        // Default constructor
        LlamaResources() : model(nullptr), ctx(nullptr), smpl(nullptr) {}
    };

    std::unique_ptr<LlamaResources> load_model(const std::string& model_path, int n_ctx = 2048, int ngl = 99);
    void generate_response(LlamaResources& resources, const std::string& prompt, std::function<void(const std::string&)> token_callback);
    void handle_conversation(LlamaResources& resources, bool is_stream = false);
    std::string handle_conversation_with_api(const std::string& prompt, PhoenixChat::LlamaResources &resources, bool is_stream, std::function<bool(const std::string&)> token_callback);
//    std::string generate_response_with_stream(LlamaResources& resources, std::function<void(const std::string &)> token_callback);

    // ************************************************************************************
//    void display_frames();
//
//    void display_loading();
//
//    std::string get_input(ConsoleState &con_st, std::string &input, chatParams &params);


//    LLModel *
//    initialize_model(const std::string &model_path, int n_ctx = 4096, int ngl = 100, const std::string &backend = "auto");
//
//    std::string handle_conversation(LLModel *model, const std::string &prompt_template, const std::string &prompt,
//                                    const chatParams &params, bool is_api_call = false,
//                                    std::function<void(const std::string &)> token_callback = nullptr);
//
//    std::string handle_conversation(LLModel *model, const std::string &prompt_template,
//                                    const std::vector<std::pair<std::string, std::string>> &chat_history,
//                                    const chatParams &params, bool is_api_call = false,
//                                    std::function<void(const std::string &)> token_callback = nullptr);
//
//    int run_command(const std::string &prompt_template, const std::string &model_path);
//
//    std::string
//    chat_with_api(const std::string &prompt_template, const std::string &model_path, const std::string &prompt);
//
//
//    std::string
//    chat_with_api_stream(const std::string &prompt_template, const std::string &model_path, const std::string &prompt,
//                         std::function<void(const std::string &)> token_callback);
//
//    std::string chat_with_api(const std::string &prompt_template, const std::string &model_path,
//                              const std::vector<std::pair<std::string, std::string>> &chat_history);
//
//    std::string chat_with_api_stream(const std::string &prompt_template, const std::string &model_path,
//                                     const std::vector<std::pair<std::string, std::string>> &chat_history,
//                                     std::function<void(const std::string &)> token_callback);
}
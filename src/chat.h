#pragma once

#include "header.h"
#include "llmodel.h"
#include "utils.h"

#include <string>


namespace PhoenixChat {
    void display_frames();

    void display_loading();

    std::string get_input(ConsoleState &con_st, std::string &input, chatParams &params);

    LLModel *
    initialize_model(const std::string &model_path, int n_ctx = 4096, int ngl = 100, const std::string &backend = "auto");

    std::string handle_conversation(LLModel *model, const std::string &prompt_template, const std::string &prompt,
                                    const chatParams &params, bool is_api_call = false,
                                    std::function<void(const std::string &)> token_callback = nullptr);

    std::string handle_conversation(LLModel *model, const std::string &prompt_template,
                                    const std::vector<std::pair<std::string, std::string>> &chat_history,
                                    const chatParams &params, bool is_api_call = false,
                                    std::function<void(const std::string &)> token_callback = nullptr);

    int run_command(const std::string &prompt_template, const std::string &model_path);

    std::string
    chat_with_api(const std::string &prompt_template, const std::string &model_path, const std::string &prompt);


    std::string
    chat_with_api_stream(const std::string &prompt_template, const std::string &model_path, const std::string &prompt,
                         std::function<void(const std::string &)> token_callback);

    std::string chat_with_api(const std::string &prompt_template, const std::string &model_path,
                              const std::vector<std::pair<std::string, std::string>> &chat_history);

    std::string chat_with_api_stream(const std::string &prompt_template, const std::string &model_path,
                                     const std::vector<std::pair<std::string, std::string>> &chat_history,
                                     std::function<void(const std::string &)> token_callback);
}
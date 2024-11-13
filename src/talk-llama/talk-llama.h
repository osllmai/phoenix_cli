#pragma once

#include "common-sdl.h"
#include "common.h"
#include "whisper.h"
#include "llama.h"

#include <vector>

namespace PhoenixTalkLlama {
    std::vector<llama_token> llama_tokenize(struct llama_context *ctx, const std::string &text, bool add_bos);

    std::string llama_token_to_piece(const struct llama_context *ctx, llama_token token);

    void whisper_print_usage(int argc, char **argv, const whisper_params &params);

    void whisper_print_usage(int /*argc*/, char **argv, const whisper_params &params);

    bool whisper_params_parse(int argc, char **argv, whisper_params &params);

    std::string transcribe(
            whisper_context *ctx,
            const whisper_params &params,
            const std::vector<float> &pcmf32,
            const std::string prompt_text,
            float &prob,
            int64_t &t_ms);

    int phoenix_talk_llama(int argc, char **argv);

    std::vector<std::string> get_words(const std::string &txt);


}
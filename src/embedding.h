#pragma once

#include "llama.h"
#include "arg.h"

namespace PhoenixEmbedding {
    std::vector<std::string> split_lines(const std::string &s, const std::string &separator = "\n");

    void batch_add_seq(llama_batch &batch, const std::vector<int32_t> &tokens, llama_seq_id seq_id);

    void batch_decode(llama_context *ctx, llama_batch &batch, float *output, int n_seq, int n_embd, int embd_norm);

    int embedding(int argc, char **argv);
}

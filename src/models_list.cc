#include "models_list.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>

using json = PhoenixModelList::json;

json PhoenixModelList::list_of_models_available() {
    json models;


    try {
        // Llama 3.1 8B Instruct 128k
        models["llama3.1-instruct"]["md5sum"] = "8a9c75bcd8a66b7693f158ec96924eeb";
        models["llama3.1-instruct"]["name"] = "Llama 3.1 8B Instruct 128k";
        models["llama3.1-instruct"]["filename"] = "Meta-Llama-3.1-8B-Instruct-128k-Q4_0.gguf";
        models["llama3.1-instruct"]["filesize"] = "4661212096";
        models["llama3.1-instruct"]["requires"] = "3.1.1";
        models["llama3.1-instruct"]["ramrequired"] = "8";
        models["llama3.1-instruct"]["parameters"] = "8 billion";
        models["llama3.1-instruct"]["quant"] = "q4_0";
        models["llama3.1-instruct"]["type"] = "LLaMA3";
        models["llama3.1-instruct"]["companyName"] = "Meta";
        models["llama3.1-instruct"]["promptTemplate"] = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2";
        models["llama3.1-instruct"]["systemPrompt"] = "<|start_header_id|>system<|end_header_id|>\nCutting Knowledge Date: December 2023\n\nYou are a helpful assistant.<|eot_id|>";
        models["llama3.1-instruct"]["url"] = "https://huggingface.co/GPT4All-Community/Meta-Llama-3.1-8B-Instruct-128k/resolve/main/Meta-Llama-3.1-8B-Instruct-128k-Q4_0.gguf";
        models["llama3.1-instruct"]["description"] = "Fast responses\n"
                                                     "Chat based model\n"
                                                     "Large context size of 128k\n"
                                                     "Accepts agentic system prompts in Llama 3.1 format\n"
                                                     "Trained by Meta\n"
                                                     "License: Meta Llama 3.1 Community License";

        // Llama 3 8B Instruct
        models["llama3-instruct"]["md5sum"] = "c87ad09e1e4c8f9c35a5fcef52b6f1c9";
        models["llama3-instruct"]["name"] = "Llama 3 8B Instruct";
        models["llama3-instruct"]["filename"] = "Meta-Llama-3-8B-Instruct.Q4_0.gguf";
        models["llama3-instruct"]["filesize"] = "4661724384";
        models["llama3-instruct"]["requires"] = "2.7.1";
        models["llama3-instruct"]["ramrequired"] = "8";
        models["llama3-instruct"]["parameters"] = "8 billion";
        models["llama3-instruct"]["quant"] = "q4_0";
        models["llama3-instruct"]["type"] = "LLaMA3";
        models["llama3-instruct"]["companyName"] = "Meta";
        models["llama3-instruct"]["promptTemplate"] = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";
        models["llama3-instruct"]["systemPrompt"] = "";
        models["llama3-instruct"]["url"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
        models["llama3-instruct"]["description"] = "Fast responses\n"
                                                   "Chat based model\n"
                                                   "Accepts system prompts in Llama 3 format\n"
                                                   "Trained by Meta\n"
                                                   "License: Meta Llama 3 Community License";

        // Nous Hermes 2 Mistral DPO
        models["nous-hermes-2-mistral-dpo"]["md5sum"] = "a5f6b4eabd3992da4d7fb7f020f921eb";
        models["nous-hermes-2-mistral-dpo"]["name"] = "Nous Hermes 2 Mistral DPO";
        models["nous-hermes-2-mistral-dpo"]["filename"] = "Nous-Hermes-2-Mistral-7B-DPO.Q4_0.gguf";
        models["nous-hermes-2-mistral-dpo"]["filesize"] = "4108928000";
        models["nous-hermes-2-mistral-dpo"]["requires"] = "2.7.1";
        models["nous-hermes-2-mistral-dpo"]["ramrequired"] = "8";
        models["nous-hermes-2-mistral-dpo"]["parameters"] = "7 billion";
        models["nous-hermes-2-mistral-dpo"]["quant"] = "q4_0";
        models["nous-hermes-2-mistral-dpo"]["type"] = "Mistral";
        models["nous-hermes-2-mistral-dpo"]["companyName"] = "Mistral";
        models["nous-hermes-2-mistral-dpo"]["promptTemplate"] = "<|im_start|>user\n%1<|im_end|>\n<|im_start|>assistant\n%2<|im_end|>\n";
        models["nous-hermes-2-mistral-dpo"]["systemPrompt"] = "";
        models["nous-hermes-2-mistral-dpo"]["url"] = "https://huggingface.co/NousResearch/Nous-Hermes-2-Mistral-7B-DPO-GGUF/resolve/main/Nous-Hermes-2-Mistral-7B-DPO.Q4_0.gguf";
        models["nous-hermes-2-mistral-dpo"]["description"] = "Good overall fast chat model\n"
                                                             "Fast responses\n"
                                                             "Chat based model\n"
                                                             "Accepts system prompts in ChatML format\n"
                                                             "Trained by Mistral AI\n"
                                                             "Finetuned by Nous Research on the OpenHermes-2.5 dataset\n"
                                                             "Licensed for commercial use";

        // Mistral Instruct
        models["mistral-instruct"]["md5sum"] = "97463be739b50525df56d33b26b00852";
        models["mistral-instruct"]["name"] = "Mistral Instruct";
        models["mistral-instruct"]["filename"] = "mistral-7b-instruct-v0.1.Q4_0.gguf";
        models["mistral-instruct"]["filesize"] = "4108916384";
        models["mistral-instruct"]["requires"] = "2.5.0";
        models["mistral-instruct"]["ramrequired"] = "8";
        models["mistral-instruct"]["parameters"] = "7 billion";
        models["mistral-instruct"]["quant"] = "q4_0";
        models["mistral-instruct"]["type"] = "Mistral";
        models["mistral-instruct"]["companyName"] = "Mistral";
        models["mistral-instruct"]["promptTemplate"] = "[INST] %1 [/INST]";
        models["mistral-instruct"]["systemPrompt"] = "";
        models["mistral-instruct"]["url"] = "https://gpt4all.io/models/gguf/mistral-7b-instruct-v0.1.Q4_0.gguf";
        models["mistral-instruct"]["description"] = "Strong overall fast instruction following model\n"
                                                    "Fast responses\n"
                                                    "Trained by Mistral AI\n"
                                                    "Uncensored\n"
                                                    "Licensed for commercial use";

        // Mistral OpenOrca
        models["mistral-openorca"]["md5sum"] = "f692417a22405d80573ac10cb0cd6c6a";
        models["mistral-openorca"]["name"] = "Mistral OpenOrca";
        models["mistral-openorca"]["filename"] = "mistral-7b-openorca.gguf2.Q4_0.gguf";
        models["mistral-openorca"]["filesize"] = "4108928128";
        models["mistral-openorca"]["requires"] = "2.7.1";
        models["mistral-openorca"]["ramrequired"] = "8";
        models["mistral-openorca"]["parameters"] = "7 billion";
        models["mistral-openorca"]["quant"] = "q4_0";
        models["mistral-openorca"]["type"] = "Mistral";
        models["mistral-openorca"]["companyName"] = "Mistral";
        models["mistral-openorca"]["promptTemplate"] = "<|im_start|>user\n%1<|im_end|>\n<|im_start|>assistant\n%2<|im_end|>\n";
        models["mistral-openorca"]["systemPrompt"] = "<|im_start|>system\nYou are MistralOrca, a large language model trained by Alignment Lab AI.\n<|im_end|>\n";
        models["mistral-openorca"]["url"] = "https://gpt4all.io/models/gguf/mistral-7b-openorca.gguf2.Q4_0.gguf";
        models["mistral-openorca"]["description"] = "Strong overall fast chat model\n"
                                                    "Fast responses\n"
                                                    "Chat based model\n"
                                                    "Trained by Mistral AI\n"
                                                    "Finetuned on OpenOrca dataset curated via Nomic Atlas\n"
                                                    "Licensed for commercial use";

        // GPT4All Falcon
        models["gpt4all-falcon"]["md5sum"] = "c4c78adf744d6a20f05c8751e3961b84";
        models["gpt4all-falcon"]["name"] = "GPT4All Falcon";
        models["gpt4all-falcon"]["filename"] = "gpt4all-falcon-newbpe-q4_0.gguf";
        models["gpt4all-falcon"]["filesize"] = "4210994112";
        models["gpt4all-falcon"]["requires"] = "2.6.0";
        models["gpt4all-falcon"]["ramrequired"] = "8";
        models["gpt4all-falcon"]["parameters"] = "7 billion";
        models["gpt4all-falcon"]["quant"] = "q4_0";
        models["gpt4all-falcon"]["type"] = "Falcon";
        models["gpt4all-falcon"]["companyName"] = "TII";
        models["gpt4all-falcon"]["promptTemplate"] = "### Instruction:\n%1\n\n### Response:\n";
        models["gpt4all-falcon"]["systemPrompt"] = "";
        models["gpt4all-falcon"]["url"] = "https://gpt4all.io/models/gguf/gpt4all-falcon-newbpe-q4_0.gguf";
        models["gpt4all-falcon"]["description"] = "Very fast model with good quality\n"
                                                  "Fastest responses\n"
                                                  "Instruction based\n"
                                                  "Trained by TII\n"
                                                  "Finetuned by Nomic AI\n"
                                                  "Licensed for commercial use";

        // Orca 2 (Medium)
        models["orca-2-medium"]["md5sum"] = "00c8593ba57f5240f59662367b3ed4a5";
        models["orca-2-medium"]["name"] = "Orca 2 (Medium)";
        models["orca-2-medium"]["filename"] = "orca-2-7b.Q4_0.gguf";
        models["orca-2-medium"]["filesize"] = "3825824192";
        models["orca-2-medium"]["requires"] = "2.5.2";
        models["orca-2-medium"]["ramrequired"] = "8";
        models["orca-2-medium"]["parameters"] = "7 billion";
        models["orca-2-medium"]["quant"] = "q4_0";
        models["orca-2-medium"]["type"] = "LLaMA2";
        models["orca-2-medium"]["companyName"] = "Microsoft";
        models["orca-2-medium"]["promptTemplate"] = "";
        models["orca-2-medium"]["systemPrompt"] = "";
        models["orca-2-medium"]["url"] = "https://gpt4all.io/models/gguf/orca-2-7b.Q4_0.gguf";
        models["orca-2-medium"]["description"] = "Instruction based\n"
                                                 "Trained by Microsoft\n"
                                                 "Cannot be used commercially";

        // Orca 2 (Full)
        models["orca-2-full"]["md5sum"] = "3c0d63c4689b9af7baa82469a6f51a19";
        models["orca-2-full"]["name"] = "Orca 2 (Full)";
        models["orca-2-full"]["filename"] = "orca-2-13b.Q4_0.gguf";
        models["orca-2-full"]["filesize"] = "7365856064";
        models["orca-2-full"]["requires"] = "2.5.2";
        models["orca-2-full"]["ramrequired"] = "16";
        models["orca-2-full"]["parameters"] = "13 billion";
        models["orca-2-full"]["quant"] = "q4_0";
        models["orca-2-full"]["type"] = "LLaMA2";
        models["orca-2-full"]["companyName"] = "Microsoft";
        models["orca-2-full"]["promptTemplate"] = "";
        models["orca-2-full"]["systemPrompt"] = "";
        models["orca-2-full"]["url"] = "https://gpt4all.io/models/gguf/orca-2-13b.Q4_0.gguf";
        models["orca-2-full"]["description"] = "Instruction based\n"
                                               "Trained by Microsoft\n"
                                               "Cannot be used commercially";

        // Wizard v1.2
        models["wizardlm-13b-v1.2"]["md5sum"] = "5aff90007499bce5c64b1c0760c0b186";
        models["wizardlm-13b-v1.2"]["name"] = "Wizard v1.2";
        models["wizardlm-13b-v1.2"]["filename"] = "wizardlm-13b-v1.2.Q4_0.gguf";
        models["wizardlm-13b-v1.2"]["filesize"] = "7365834624";
        models["wizardlm-13b-v1.2"]["requires"] = "2.5.0";
        models["wizardlm-13b-v1.2"]["ramrequired"] = "16";
        models["wizardlm-13b-v1.2"]["parameters"] = "13 billion";
        models["wizardlm-13b-v1.2"]["quant"] = "q4_0";
        models["wizardlm-13b-v1.2"]["type"] = "LLaMA2";
        models["wizardlm-13b-v1.2"]["companyName"] = "Microsoft";
        models["wizardlm-13b-v1.2"]["promptTemplate"] = "";
        models["wizardlm-13b-v1.2"]["systemPrompt"] = "";
        models["wizardlm-13b-v1.2"]["url"] = "https://gpt4all.io/models/gguf/wizardlm-13b-v1.2.Q4_0.gguf";
        models["wizardlm-13b-v1.2"]["description"] = "Strong overall larger model\n"
                                                     "Instruction based\n"
                                                     "Gives very long responses\n"
                                                     "Finetuned with only 1k of high-quality data\n"
                                                     "Trained by Microsoft and Peking University\n"
                                                     "Cannot be used commercially";

        // Ghost 7B v0.9.1
        models["ghost-7b-v0.9.1"]["md5sum"] = "31b47b4e8c1816b62684ac3ca373f9e1";
        models["ghost-7b-v0.9.1"]["name"] = "Ghost 7B v0.9.1";
        models["ghost-7b-v0.9.1"]["filename"] = "ghost-7b-v0.9.1-Q4_0.gguf";
        models["ghost-7b-v0.9.1"]["filesize"] = "4108916960";
        models["ghost-7b-v0.9.1"]["requires"] = "2.7.1";
        models["ghost-7b-v0.9.1"]["ramrequired"] = "8";
        models["ghost-7b-v0.9.1"]["parameters"] = "7 billion";
        models["ghost-7b-v0.9.1"]["quant"] = "q4_0";
        models["ghost-7b-v0.9.1"]["type"] = "Mistral";
        models["ghost-7b-v0.9.1"]["companyName"] = "Mistral";
        models["ghost-7b-v0.9.1"]["promptTemplate"] = "<|user|>\n%1</s>\n<|assistant|>\n%2</s>\n";
        models["ghost-7b-v0.9.1"]["systemPrompt"] = "<|system|>\nYou are Ghost created by Lam Hieu. You are a helpful and knowledgeable assistant. You like to help and always give honest information, in its original language. In communication, you are always respectful, equal and promote positive behavior.\n</s>";
        models["ghost-7b-v0.9.1"]["url"] = "https://huggingface.co/lamhieu/ghost-7b-v0.9.1-gguf/resolve/main/ghost-7b-v0.9.1-Q4_0.gguf";
        models["ghost-7b-v0.9.1"]["description"] = "Ghost 7B v0.9.1 fast, powerful and smooth for Vietnamese and English languages.";

    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed while creating model list: " << e.what() << std::endl;
        throw;

    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON processing error while creating model list: " << e.what() << std::endl;
        throw std::runtime_error("Error constructing JSON model list");

    } catch (const std::exception& e) {
        std::cerr << "Unexpected error occurred while creating model list: " << e.what() << std::endl;
        throw;

    } catch (...) {
        std::cerr << "Unknown error occurred while creating model list." << std::endl;
        throw std::runtime_error("Unknown error in model list construction");
    }

    return models;
}

std::string PhoenixModelList::get_url_llm_download(const std::string &model_name) {
    json models = list_of_models_available();
    if (models.contains(model_name)) {
        return models[model_name]["url"];
    } else {
        throw std::invalid_argument("Model not found");
    }
}

json PhoenixModelList::model_data(const std::string &model_name) {
    json models = list_of_models_available();
    if (models.contains(model_name)) {
        return models[model_name];
    } else {
        throw std::invalid_argument("Model data not available for the specified model name");
    }
}

#include "models_list.h"
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

json list_of_models_available() {
    json j;
// Llama 3.1 8B Instruct 128k
    j["llama3.1-instruct"]["md5sum"] = "8a9c75bcd8a66b7693f158ec96924eeb";
    j["llama3.1-instruct"]["name"] = "Llama 3.1 8B Instruct 128k";
    j["llama3.1-instruct"]["filename"] = "Meta-Llama-3.1-8B-Instruct-128k-Q4_0.gguf";
    j["llama3.1-instruct"]["filesize"] = "4661212096";
    j["llama3.1-instruct"]["requires"] = "3.1.1";
    j["llama3.1-instruct"]["ramrequired"] = "8";
    j["llama3.1-instruct"]["parameters"] = "8 billion";
    j["llama3.1-instruct"]["quant"] = "q4_0";
    j["llama3.1-instruct"]["type"] = "LLaMA3";
    j["llama3.1-instruct"]["companyName"] = "Meta";
    j["llama3.1-instruct"]["promptTemplate"] = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2";
    j["llama3.1-instruct"]["systemPrompt"] = "<|start_header_id|>system<|end_header_id|>\nCutting Knowledge Date: December 2023\n\nYou are a helpful assistant.<|eot_id|>";
    j["llama3.1-instruct"]["url"] = "https://huggingface.co/GPT4All-Community/Meta-Llama-3.1-8B-Instruct-128k/resolve/main/Meta-Llama-3.1-8B-Instruct-128k-Q4_0.gguf";
    j["llama3.1-instruct"]["description"] = "Fast responses\n"
                                            "Chat based model\n"
                                            "Large context size of 128k\n"
                                            "Accepts agentic system prompts in Llama 3.1 format\n"
                                            "Trained by Meta\n"
                                            "License: Meta Llama 3.1 Community License";

    // Llama 3 8B Instruct
    j["llama3-instruct"]["md5sum"] = "c87ad09e1e4c8f9c35a5fcef52b6f1c9";
    j["llama3-instruct"]["name"] = "Llama 3 8B Instruct";
    j["llama3-instruct"]["filename"] = "Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    j["llama3-instruct"]["filesize"] = "4661724384";
    j["llama3-instruct"]["requires"] = "2.7.1";
    j["llama3-instruct"]["ramrequired"] = "8";
    j["llama3-instruct"]["parameters"] = "8 billion";
    j["llama3-instruct"]["quant"] = "q4_0";
    j["llama3-instruct"]["type"] = "LLaMA3";
    j["llama3-instruct"]["companyName"] = "Meta";
    j["llama3-instruct"]["promptTemplate"] = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";
    j["llama3-instruct"]["systemPrompt"] = "";
    j["llama3-instruct"]["url"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    j["llama3-instruct"]["description"] = "Fast responses\n"
                                          "Chat based model\n"
                                          "Accepts system prompts in Llama 3 format\n"
                                          "Trained by Meta\n"
                                          "License: Meta Llama 3 Community License";

    // Nous Hermes 2 Mistral DPO
    j["nous-hermes-2-mistral-dpo"]["md5sum"] = "a5f6b4eabd3992da4d7fb7f020f921eb";
    j["nous-hermes-2-mistral-dpo"]["name"] = "Nous Hermes 2 Mistral DPO";
    j["nous-hermes-2-mistral-dpo"]["filename"] = "Nous-Hermes-2-Mistral-7B-DPO.Q4_0.gguf";
    j["nous-hermes-2-mistral-dpo"]["filesize"] = "4108928000";
    j["nous-hermes-2-mistral-dpo"]["requires"] = "2.7.1";
    j["nous-hermes-2-mistral-dpo"]["ramrequired"] = "8";
    j["nous-hermes-2-mistral-dpo"]["parameters"] = "7 billion";
    j["nous-hermes-2-mistral-dpo"]["quant"] = "q4_0";
    j["nous-hermes-2-mistral-dpo"]["type"] = "Mistral";
    j["nous-hermes-2-mistral-dpo"]["companyName"] = "Mistral";
    j["nous-hermes-2-mistral-dpo"]["promptTemplate"] = "<|im_start|>user\n%1<|im_end|>\n<|im_start|>assistant\n%2<|im_end|>\n";
    j["nous-hermes-2-mistral-dpo"]["systemPrompt"] = "";
    j["nous-hermes-2-mistral-dpo"]["url"] = "https://huggingface.co/NousResearch/Nous-Hermes-2-Mistral-7B-DPO-GGUF/resolve/main/Nous-Hermes-2-Mistral-7B-DPO.Q4_0.gguf";
    j["nous-hermes-2-mistral-dpo"]["description"] = "Good overall fast chat model\n"
                                                    "Fast responses\n"
                                                    "Chat based model\n"
                                                    "Accepts system prompts in ChatML format\n"
                                                    "Trained by Mistral AI\n"
                                                    "Finetuned by Nous Research on the OpenHermes-2.5 dataset\n"
                                                    "Licensed for commercial use";

    // Mistral Instruct
    j["mistral-instruct"]["md5sum"] = "97463be739b50525df56d33b26b00852";
    j["mistral-instruct"]["name"] = "Mistral Instruct";
    j["mistral-instruct"]["filename"] = "mistral-7b-instruct-v0.1.Q4_0.gguf";
    j["mistral-instruct"]["filesize"] = "4108916384";
    j["mistral-instruct"]["requires"] = "2.5.0";
    j["mistral-instruct"]["ramrequired"] = "8";
    j["mistral-instruct"]["parameters"] = "7 billion";
    j["mistral-instruct"]["quant"] = "q4_0";
    j["mistral-instruct"]["type"] = "Mistral";
    j["mistral-instruct"]["companyName"] = "Mistral";
    j["mistral-instruct"]["promptTemplate"] = "[INST] %1 [/INST]";
    j["mistral-instruct"]["systemPrompt"] = "";
    j["mistral-instruct"]["url"] = "https://gpt4all.io/models/gguf/mistral-7b-instruct-v0.1.Q4_0.gguf";
    j["mistral-instruct"]["description"] = "Strong overall fast instruction following model\n"
                                           "Fast responses\n"
                                           "Trained by Mistral AI\n"
                                           "Uncensored\n"
                                           "Licensed for commercial use";

    // Mistral OpenOrca
    j["mistral-openorca"]["md5sum"] = "f692417a22405d80573ac10cb0cd6c6a";
    j["mistral-openorca"]["name"] = "Mistral OpenOrca";
    j["mistral-openorca"]["filename"] = "mistral-7b-openorca.gguf2.Q4_0.gguf";
    j["mistral-openorca"]["filesize"] = "4108928128";
    j["mistral-openorca"]["requires"] = "2.7.1";
    j["mistral-openorca"]["ramrequired"] = "8";
    j["mistral-openorca"]["parameters"] = "7 billion";
    j["mistral-openorca"]["quant"] = "q4_0";
    j["mistral-openorca"]["type"] = "Mistral";
    j["mistral-openorca"]["companyName"] = "Mistral";
    j["mistral-openorca"]["promptTemplate"] = "<|im_start|>user\n%1<|im_end|>\n<|im_start|>assistant\n%2<|im_end|>\n";
    j["mistral-openorca"]["systemPrompt"] = "<|im_start|>system\nYou are MistralOrca, a large language model trained by Alignment Lab AI.\n<|im_end|>\n";
    j["mistral-openorca"]["url"] = "https://gpt4all.io/models/gguf/mistral-7b-openorca.gguf2.Q4_0.gguf";
    j["mistral-openorca"]["description"] = "Strong overall fast chat model\n"
                                           "Fast responses\n"
                                           "Chat based model\n"
                                           "Trained by Mistral AI\n"
                                           "Finetuned on OpenOrca dataset curated via Nomic Atlas\n"
                                           "Licensed for commercial use";

    // GPT4All Falcon
    j["gpt4all-falcon"]["md5sum"] = "c4c78adf744d6a20f05c8751e3961b84";
    j["gpt4all-falcon"]["name"] = "GPT4All Falcon";
    j["gpt4all-falcon"]["filename"] = "gpt4all-falcon-newbpe-q4_0.gguf";
    j["gpt4all-falcon"]["filesize"] = "4210994112";
    j["gpt4all-falcon"]["requires"] = "2.6.0";
    j["gpt4all-falcon"]["ramrequired"] = "8";
    j["gpt4all-falcon"]["parameters"] = "7 billion";
    j["gpt4all-falcon"]["quant"] = "q4_0";
    j["gpt4all-falcon"]["type"] = "Falcon";
    j["gpt4all-falcon"]["companyName"] = "TII";
    j["gpt4all-falcon"]["promptTemplate"] = "### Instruction:\n%1\n\n### Response:\n";
    j["gpt4all-falcon"]["systemPrompt"] = "";
    j["gpt4all-falcon"]["url"] = "https://gpt4all.io/models/gguf/gpt4all-falcon-newbpe-q4_0.gguf";
    j["gpt4all-falcon"]["description"] = "Very fast model with good quality\n"
                                         "Fastest responses\n"
                                         "Instruction based\n"
                                         "Trained by TII\n"
                                         "Finetuned by Nomic AI\n"
                                         "Licensed for commercial use";

    // Orca 2 (Medium)
    j["orca-2-medium"]["md5sum"] = "00c8593ba57f5240f59662367b3ed4a5";
    j["orca-2-medium"]["name"] = "Orca 2 (Medium)";
    j["orca-2-medium"]["filename"] = "orca-2-7b.Q4_0.gguf";
    j["orca-2-medium"]["filesize"] = "3825824192";
    j["orca-2-medium"]["requires"] = "2.5.2";
    j["orca-2-medium"]["ramrequired"] = "8";
    j["orca-2-medium"]["parameters"] = "7 billion";
    j["orca-2-medium"]["quant"] = "q4_0";
    j["orca-2-medium"]["type"] = "LLaMA2";
    j["orca-2-medium"]["companyName"] = "Microsoft";
    j["orca-2-medium"]["promptTemplate"] = "";
    j["orca-2-medium"]["systemPrompt"] = "";
    j["orca-2-medium"]["url"] = "https://gpt4all.io/models/gguf/orca-2-7b.Q4_0.gguf";
    j["orca-2-medium"]["description"] = "Instruction based\n"
                                        "Trained by Microsoft\n"
                                        "Cannot be used commercially";

    // Orca 2 (Full)
    j["orca-2-full"]["md5sum"] = "3c0d63c4689b9af7baa82469a6f51a19";
    j["orca-2-full"]["name"] = "Orca 2 (Full)";
    j["orca-2-full"]["filename"] = "orca-2-13b.Q4_0.gguf";
    j["orca-2-full"]["filesize"] = "7365856064";
    j["orca-2-full"]["requires"] = "2.5.2";
    j["orca-2-full"]["ramrequired"] = "16";
    j["orca-2-full"]["parameters"] = "13 billion";
    j["orca-2-full"]["quant"] = "q4_0";
    j["orca-2-full"]["type"] = "LLaMA2";
    j["orca-2-full"]["companyName"] = "Microsoft";
    j["orca-2-full"]["promptTemplate"] = "";
    j["orca-2-full"]["systemPrompt"] = "";
    j["orca-2-full"]["url"] = "https://gpt4all.io/models/gguf/orca-2-13b.Q4_0.gguf";
    j["orca-2-full"]["description"] = "Instruction based\n"
                                      "Trained by Microsoft\n"
                                      "Cannot be used commercially";

    // Wizard v1.2
    j["wizardlm-13b-v1.2"]["md5sum"] = "5aff90007499bce5c64b1c0760c0b186";
    j["wizardlm-13b-v1.2"]["name"] = "Wizard v1.2";
    j["wizardlm-13b-v1.2"]["filename"] = "wizardlm-13b-v1.2.Q4_0.gguf";
    j["wizardlm-13b-v1.2"]["filesize"] = "7365834624";
    j["wizardlm-13b-v1.2"]["requires"] = "2.5.0";
    j["wizardlm-13b-v1.2"]["ramrequired"] = "16";
    j["wizardlm-13b-v1.2"]["parameters"] = "13 billion";
    j["wizardlm-13b-v1.2"]["quant"] = "q4_0";
    j["wizardlm-13b-v1.2"]["type"] = "LLaMA2";
    j["wizardlm-13b-v1.2"]["companyName"] = "Microsoft";
    j["wizardlm-13b-v1.2"]["promptTemplate"] = "";
    j["wizardlm-13b-v1.2"]["systemPrompt"] = "";
    j["wizardlm-13b-v1.2"]["url"] = "https://gpt4all.io/models/gguf/wizardlm-13b-v1.2.Q4_0.gguf";
    j["wizardlm-13b-v1.2"]["description"] = "Strong overall larger model\n"
                                            "Instruction based\n"
                                            "Gives very long responses\n"
                                            "Finetuned with only 1k of high-quality data\n"
                                            "Trained by Microsoft and Peking University\n"
                                            "Cannot be used commercially";

    // Ghost 7B v0.9.1
    j["ghost-7b-v0.9.1"]["md5sum"] = "31b47b4e8c1816b62684ac3ca373f9e1";
    j["ghost-7b-v0.9.1"]["name"] = "Ghost 7B v0.9.1";
    j["ghost-7b-v0.9.1"]["filename"] = "ghost-7b-v0.9.1-Q4_0.gguf";
    j["ghost-7b-v0.9.1"]["filesize"] = "4108916960";
    j["ghost-7b-v0.9.1"]["requires"] = "2.7.1";
    j["ghost-7b-v0.9.1"]["ramrequired"] = "8";
    j["ghost-7b-v0.9.1"]["parameters"] = "7 billion";
    j["ghost-7b-v0.9.1"]["quant"] = "q4_0";
    j["ghost-7b-v0.9.1"]["type"] = "Mistral";
    j["ghost-7b-v0.9.1"]["companyName"] = "Mistral";
    j["ghost-7b-v0.9.1"]["promptTemplate"] = "<|user|>\n%1</s>\n<|assistant|>\n%2</s>\n";
    j["ghost-7b-v0.9.1"]["systemPrompt"] = "<|system|>\nYou are Ghost created by Lam Hieu. You are a helpful and knowledgeable assistant. You like to help and always give honest information, in its original language. In communication, you are always respectful, equal and promote positive behavior.\n</s>";
    j["ghost-7b-v0.9.1"]["url"] = "https://huggingface.co/lamhieu/ghost-7b-v0.9.1-gguf/resolve/main/ghost-7b-v0.9.1-Q4_0.gguf";
    j["ghost-7b-v0.9.1"]["description"] = "Ghost 7B v0.9.1 fast, powerful and smooth for Vietnamese and English languages.";




    return j;
}

std::string get_url_llm_download(const std::string &model_name) {
    json models_list = list_of_models_available();
    if (models_list.contains(model_name) && models_list[model_name].contains("url")) {
        return models_list[model_name]["url"].get<std::string>();
    } else {
        std::cerr << "Error: Model name '" << model_name << "' not found." << std::endl;
        return "";
    }
}

json model_data(const std::string &model_name) {
    json models_list = list_of_models_available();
    if (models_list.contains(model_name)) {
        return models_list[model_name];
    } else {
        std::cerr << "Error: Model name '" << model_name << "' not found." << std::endl;
        return json::object(); // Return an empty JSON object if the model is not found
    }
}

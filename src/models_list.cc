#include "models_list.h"

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

std::map<std::string, std::string> list_of_models_available() {
    std::map<std::string, std::string> models_list;
    models_list["llama3-instruct"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    models_list["orca2"] = "https://gpt4all.io/models/gguf/orca-2-7b.Q4_0.gguf";
    return models_list;
};

std::string get_url_llm_download(const std::string &model_name) {
    std::map<std::string, std::string> models_list = list_of_models_available();

    try {
        return models_list.at(model_name);
    }
    catch (const std::out_of_range &e) {
        std::cerr << "Error: Model name '" << model_name << "' not found." << std::endl;
        return "";
    }
}
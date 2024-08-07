#include "models_list.h"
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

json list_of_models_available() {
    json j;
    j["llama3-instruct"]["companyName"] = "Meta";
    j["llama3-instruct"]["url"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    j["orca2"]["companyName"] = "Microsoft";
    j["orca2"]["url"] = "https://gpt4all.io/models/gguf/orca-2-7b.Q4_0.gguf";

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

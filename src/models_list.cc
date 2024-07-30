#include "models_list.h"

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

std::string list_of_models_available(const std::string &model_name)
{
    std::map<std::string, std::string> models_list;
    models_list["llama3-8b"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    models_list["llama3-70b"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    models_list["phi"] = "https://gpt4all.io/models/gguf/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
    try
    {
        return models_list.at(model_name);
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: Model name '" << model_name << "' not found." << std::endl;
        return "";
    }
}
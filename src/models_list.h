#pragma once

#include <nlohmann/json.hpp>
#include <string>

class PhoenixModelList {
public:
    using json = nlohmann::json;

    static json list_of_models_available();
    static std::string get_url_llm_download(const std::string &model_name);
    static json model_data(const std::string &model_name);
};

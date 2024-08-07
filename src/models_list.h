#pragma once
#include <nlohmann/json.hpp>

#include <string>
#include <map>

using json = nlohmann::json;

json list_of_models_available();

std::string get_url_llm_download(const std::string &model_name);

json model_data(const std::string &model_name);
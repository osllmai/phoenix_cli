#pragma once

#include <string>
#include <map>

std::map<std::string, std::string> list_of_models_available();

std::string get_url_llm_download(const std::string &model_name);
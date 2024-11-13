#pragma once

#include "header.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>


class PhornixChatManager {
public:
    using json = nlohmann::json;

    static std::string generate_unique_id();

    static bool create_chat_config_file(const std::string &id);

    static std::string save_chat_history(const std::string &id, const std::string &prompt, const std::string &answer);

    static std::vector<std::string> chat_histories();

    static json chat_history_conversation(const std::string &path);

};

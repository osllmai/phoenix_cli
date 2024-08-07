//
// Created by Amir Kouhkan on 8/6/24.
//

#ifndef PHOENIX_CHAT_MANAGER_H
#define PHOENIX_CHAT_MANAGER_H

#include "header.h"

#include <string>

class ChatManager {
public:
    static std::string generate_unique_id();

    static bool create_chat_config_file(const std::string &id, chatParams &params);

    static bool save_chat_history(const std::string &id, const std::string &prompt, const std::string &answer);


};


#endif //PHOENIX_CHAT_MANAGER_H

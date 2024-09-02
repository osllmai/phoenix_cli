//
// Created by Amir Kouhkan on 9/2/24.
//

#pragma once

#include <string>

namespace models {
    class Profile {
    public:
        static void create(const int &user_id, const std::string &display_name);
    };
}

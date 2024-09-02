//
// Created by Amir Kouhkan on 9/1/24.
//

#pragma once

#include <string>

namespace models {
    class User {
    public:
        static int create(const std::string &email, const std::string &password);

        static bool authenticate(const std::string &email, const std::string &password);

        static bool reset_password(const std::string &email, const std::string &old_password, const std::string &new_password);
    };
}

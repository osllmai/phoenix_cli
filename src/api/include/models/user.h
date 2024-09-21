//
// Created by Amir Kouhkan on 9/1/24.
//

#pragma once

#include <string>

struct BaseUser {
    int id;
    std::string username;
    std::string email;
};

namespace models {
    class User {
    public:
        static int create(const std::string &email, const std::string &password);

        static bool authenticate(const std::string &email, const std::string &password);

        static bool reset_password(const std::string &email, const std::string &old_password, const std::string &new_password);

        static BaseUser get_user_by_email(const std::string &email);


    };
}

#pragma once

#include <string>

std::string get_current_time();

std::optional<std::string> get_user_id_from_token(const std::string &token);

std::string create_jwt(const int &user_id, bool is_refresh_token = false);

bool verify_jwt(const std::string &token);
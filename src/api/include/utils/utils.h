#pragma once

#include <string>

std::string get_current_time();

std::optional<std::string> get_user_id_from_token(const std::string &token);

std::string create_jwt(const int &user_id, bool is_refresh_token = false);

bool verify_jwt(const std::string &token);

std::string serialize_image_paths(const std::vector<std::string>& image_paths);

std::vector<std::string> deserialize_image_paths(const std::string& image_paths_str);
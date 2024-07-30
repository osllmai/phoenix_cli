#pragma once

#include <iostream>
#include <string>

// Callback function writes data to a file
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

// Declare function to implement download model form internet
bool download_model_file(const std::string &url, const std::string &model_path);
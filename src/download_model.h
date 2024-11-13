#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>

class PhoenixDownloadModel {
public:
    PhoenixDownloadModel(const std::string &url, const std::string &target_path);

    bool download();

private:
    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

    static int progress_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    static std::string human_readable_size(curl_off_t bytes);

    static bool is_directory(const std::string &path);

    std::string url;
    std::string target_path;
    std::ofstream outstream;

    struct ProgressData {
        CURL *curl;
        double lastruntime;
    };

    ProgressData progress_data;
};

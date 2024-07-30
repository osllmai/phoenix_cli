#include "download_model.h"
#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <sys/stat.h>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cstdio>

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 3 

struct myprogress
{
    double lastruntime; 
    CURL *curl;
};

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    std::ofstream *outstream = (std::ofstream *)userp;
    size_t total_size = size * nmemb;
    outstream->write((char *)contents, total_size);
    return total_size;
}

std::string human_readable_size(curl_off_t bytes)
{
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    char buffer[50];
    int i = 0;
    double dblBytes = bytes;

    if (bytes > 1024)
    {
        for (i = 0; (bytes / 1024) > 0 && i < 4; i++, bytes /= 1024)
            dblBytes = bytes / 1024.0;
    }

    snprintf(buffer, sizeof(buffer), "%.2f %s", dblBytes, suffixes[i]);
    return std::string(buffer);
}

int progress_callback(void *p,
                      curl_off_t dltotal, curl_off_t dlnow,
                      curl_off_t ultotal, curl_off_t ulnow)
{
    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    double curtime = 0.0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);


    std::string downloaded = human_readable_size(dlnow);
    std::string total = human_readable_size(dltotal);

    std::cout << "DOWNLOADED: " << downloaded << " / " << total << "\r" << std::flush;

    return 0;
}

bool is_directory(const std::string &path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
    {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

bool download_model_file(const std::string &url, const std::string &model_path)
{
    if (is_directory(model_path))
    {
        std::cerr << model_path << " is a directory, not a file" << std::endl;
        return false;
    }

    CURL *curl;
    CURLcode res = CURLE_OK;
    struct myprogress prog;
    std::ofstream outstream(model_path, std::ios::binary);

    if (!outstream.is_open())
    {
        std::cerr << "Failed to open file for writing: " << model_path << std::endl;
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        prog.curl = curl;
        prog.lastruntime = 0.0; 

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outstream);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }

    curl_global_cleanup();
    return true;
}

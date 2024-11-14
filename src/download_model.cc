#include "download_model.h"
#include <sys/stat.h>
#include <iomanip>
#include <cstdio>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define snprintf sprintf_s
#else
#include <unistd.h>
#endif

PhoenixDownloadModel::PhoenixDownloadModel(const std::string &url, const std::string &target_path)
        : url(url), target_path(target_path) {}

size_t PhoenixDownloadModel::write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::ofstream *outstream = static_cast<std::ofstream *>(userp);
    size_t total_size = size * nmemb;
    if (outstream->write(static_cast<char *>(contents), total_size)) {
        return total_size;
    } else {
        std::cerr << "Error writing to output stream" << std::endl;
        return 0; // Returning 0 will signal CURL to abort the download
    }
}

std::string PhoenixDownloadModel::human_readable_size(curl_off_t bytes) {
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblBytes = bytes;

    while (bytes > 1024 && i < 4) {
        dblBytes = bytes / 1024.0;
        bytes /= 1024;
        ++i;
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.2f %s", dblBytes, suffixes[i]);
    return std::string(buffer);
}

int PhoenixDownloadModel::progress_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    ProgressData *prog_data = static_cast<ProgressData *>(p);
    CURL *curl = prog_data->curl;

    double curtime = 0.0;
    if (curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime) != CURLE_OK) {
        std::cerr << "Failed to retrieve total time information from CURL" << std::endl;
        return 1; // Return non-zero to indicate an error and abort the download
    }

    std::cout << "DOWNLOADED: " << human_readable_size(dlnow) << " / " << human_readable_size(dltotal) << "\r"
              << std::flush;

    return 0;
}

bool PhoenixDownloadModel::is_directory(const std::string &path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        std::cerr << "Error checking if path is a directory: " << strerror(errno) << std::endl;
        return false;
    }
    return (statbuf.st_mode & S_IFDIR) != 0;
}

bool PhoenixDownloadModel::download() {
    try {
        if (is_directory(target_path)) {
            std::cerr << target_path << " is a directory, not a file" << std::endl;
            return false;
        }

        outstream.open(target_path, std::ios::binary);
        if (!outstream.is_open()) {
            std::cerr << "Failed to open file for writing: " << target_path << std::endl;
            return false;
        }

        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl) {
            progress_data.curl = curl;
            progress_data.lastruntime = 0.0;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outstream);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress_data);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return false;
            }

            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize CURL" << std::endl;
            curl_global_cleanup();
            return false;
        }

        curl_global_cleanup();
        return true;

    } catch (const std::exception &e) {
        std::cerr << "Exception during download: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error occurred during download" << std::endl;
        return false;
    }
}

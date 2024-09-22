#pragma once

#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using json = nlohmann::json;

class OpenAI {
public:
    explicit OpenAI(std::string api_key);
    ~OpenAI();

    OpenAI(const OpenAI&) = delete;
    OpenAI& operator=(const OpenAI&) = delete;
    OpenAI(OpenAI&&) noexcept;
    OpenAI& operator=(OpenAI&&) noexcept;

    void stream_chat_completion(const std::string& model,
                                const json& messages,
                                const std::function<bool(const std::string&)>& callback,
                                std::atomic<bool>& connection_alive);

private:
    struct CallbackData {
        std::string response_string;
        std::function<bool(const std::string&)> user_callback;
        std::atomic<bool>& connection_alive;
    };

    std::string m_api_key;
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> m_curl;

    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* user_data);

    void init_curl();
};
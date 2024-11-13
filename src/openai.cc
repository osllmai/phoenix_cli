#include "openai.h"
#include <iostream>
#include <stdexcept>

OpenAI::OpenAI(std::string api_key)
        : m_api_key(std::move(api_key)),
          m_curl(nullptr, curl_easy_cleanup)
{
    init_curl();
}

OpenAI::~OpenAI() = default;
OpenAI::OpenAI(OpenAI&&) noexcept = default;
OpenAI& OpenAI::operator=(OpenAI&&) noexcept = default;

void OpenAI::stream_chat_completion(const std::string& model,
                                    const PhoenixOpenAI::json& messages,
                                    const std::function<bool(const std::string&)>& callback,
                                    std::atomic<bool>& connection_alive) {
    const std::string url = "https://api.openai.com/v1/chat/completions";

    PhoenixOpenAI::json payload = {
            {"model", model},
            {"messages", messages},
            {"stream", true}
    };

    std::string payload_str = payload.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + m_api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);

    CallbackData callback_data = {std::string(), callback, connection_alive};

    curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, &callback_data);

    CURLcode res = curl_easy_perform(m_curl.get());

    curl_slist_free_all(headers);

    if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
        throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
    }
}

void OpenAI::init_curl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    m_curl.reset(curl_easy_init());
    if (!m_curl) {
        throw std::runtime_error("Failed to initialize libcurl");
    }
}

size_t OpenAI::write_callback(char* ptr, size_t size, size_t nmemb, void* user_data) {
    auto* data = static_cast<CallbackData*>(user_data);
    if (!data->connection_alive) return 0;  // Stop processing if connection is closed

    size_t realsize = size * nmemb;
    data->response_string.append(ptr, realsize);

    size_t pos;
    while ((pos = data->response_string.find('\n')) != std::string::npos) {
        std::string line = data->response_string.substr(0, pos);
        data->response_string.erase(0, pos + 1);

        if (line.starts_with("data: ")) {
            line = line.substr(6);
            if (line != "[DONE]") {
                try {
                    PhoenixOpenAI::json chunk = PhoenixOpenAI::json::parse(line);
                    if (chunk["choices"][0].contains("delta")) {
                        auto& delta = chunk["choices"][0]["delta"];
                        if (delta.contains("content") && !delta["content"].is_null()) {
                            std::string content = delta["content"].get<std::string>();
                            if (!content.empty()) {
                                if (!data->user_callback(content)) {
                                    data->connection_alive = false;
                                    return 0;  // Stop processing if callback returns false
                                }
                            }
                        }
                    }
                } catch (PhoenixOpenAI::json::exception& e) {
                    std::cerr << "JSON error: " << e.what() << std::endl;
                }
            }
        }
    }

    return realsize;
}
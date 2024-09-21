#include "web_server.h"
#include "chat.h"
#include "directory_manager.h"
#include "models_list.h"
#include "download_model.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using json = nlohmann::json;
using tcp = net::ip::tcp;

// Helper function to convert chat history to string
std::string chat_history_to_string(const std::vector<std::pair<std::string, std::string>> &chat_history) {
    std::string result;
    for (const auto &pair: chat_history) {
        result += pair.first + ": " + pair.second + "\n";
    }
    return result;
}

// Function to send a streaming response
void send_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                             const std::string &path, const std::string &prompt, bool keep_alive, int version) {
    http::response<http::empty_body> res{http::status::ok, version};
    res.set(http::field::server, "Beast");
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::access_control_allow_headers, "content-type");
    res.set(http::field::access_control_allow_methods, "POST");
    res.set(http::field::content_type, "application/json");
    res.set(http::field::transfer_encoding, "chunked");
    res.keep_alive(keep_alive);

    // Send the headers
    http::serializer<false, http::empty_body> sr{res};
    http::write_header(*socket, sr);

    auto send_chunk = [socket](const std::string &data) {
        std::string chunk = data;
        std::stringstream ss;
        ss << std::hex << chunk.size() << "\r\n" << chunk << "\r\n";
        std::string formatted_chunk = ss.str();

        boost::system::error_code ec;
        boost::asio::write(*socket, boost::asio::buffer(formatted_chunk), ec);
        if (ec) {
            std::cerr << "Error sending chunk: " << ec.message() << std::endl;
            return false;
        }
        return true;
    };

    auto token_callback = [&send_chunk](const std::string &token) -> bool {
        std::string json_chunk = "{\"chunk\": \"" + token + "\"}";
        return send_chunk(json_chunk);
    };

    PhoenixChat::chat_with_api_stream(prompt_template, path, prompt, token_callback);

    // Send the final chunk to indicate the end of the stream
    send_chunk("[DONE]");

    // Send the terminating chunk
    boost::system::error_code ec;
    boost::asio::write(*socket, boost::asio::buffer("0\r\n\r\n"), ec);
    if (ec) {
        std::cerr << "Error sending terminating chunk: " << ec.message() << std::endl;
    }
}

// Function to send a non-streaming response
void send_non_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                                 const std::string &path, const std::string &prompt, bool keep_alive, int version) {
    std::string api_response = PhoenixChat::chat_with_api(prompt_template, path, prompt);

    boost::property_tree::ptree response_pt;
    response_pt.put("response", api_response);
    std::ostringstream json_response_stream;
    boost::property_tree::write_json(json_response_stream, response_pt);
    std::string json_response = json_response_stream.str();

    http::response<http::string_body> res{http::status::ok, version};
    res.set(http::field::server, "Beast");
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::access_control_allow_headers, "content-type");
    res.set(http::field::access_control_allow_methods, "POST");
    res.set(http::field::content_type, "application/json");
    res.body() = json_response;
    res.prepare_payload();
    res.keep_alive(keep_alive);

    http::write(*socket, res);
}

// Function to send a bad request response
void send_bad_request(std::shared_ptr<tcp::socket> socket, bool keep_alive, int version) {
    http::response<http::string_body> res{http::status::bad_request, version};
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"error": "Invalid request"})";
    res.prepare_payload();
    res.keep_alive(keep_alive);

    http::write(*socket, res);
}

// Function to send an internal server error response
void send_internal_server_error(std::shared_ptr<tcp::socket> socket) {
    http::response<http::string_body> res{http::status::internal_server_error, 11};
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"error": "Internal server error"})";
    res.prepare_payload();

    http::write(*socket, res);
}

// Function to handle /api/generate requests
void handle_generate_request(std::shared_ptr<tcp::socket> socket, const http::request<http::dynamic_body> &req,
                             bool keep_alive) {
    std::string body = beast::buffers_to_string(req.body().data());
    std::istringstream json_stream(body);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(json_stream, pt);

    std::string model_name = pt.get<std::string>("model");
    std::string prompt = pt.get<std::string>("prompt");
    bool stream = pt.get<bool>("stream");

    std::cout << model_name << std::endl;
    std::cout << prompt << std::endl;

    json data = model_data(model_name);
    std::string path;
    std::string prompt_template = data["promptTemplate"].get<std::string>();

    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        path = DatabaseManager::get_path_by_model_name(db, model_name);
        sqlite3_close(db);
    }

    if (stream) {
        send_streaming_response(socket, prompt_template, path, prompt, keep_alive, req.version());
    } else {
        send_non_streaming_response(socket, prompt_template, path, prompt, keep_alive, req.version());
    }
}

// Function to handle /api/chat requests
void handle_chat_request(std::shared_ptr<tcp::socket> socket, const http::request<http::dynamic_body> &req,
                         bool keep_alive) {
    std::string body = beast::buffers_to_string(req.body().data());
    std::istringstream json_stream(body);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(json_stream, pt);

    std::string model_name = pt.get<std::string>("model");
    bool stream = pt.get<bool>("stream");

    // Extract chat history
    std::vector<std::pair<std::string, std::string>> chat_history;
    for (const auto &item: pt.get_child("messages")) {
        std::string role = item.second.get<std::string>("role");
        std::string prompt = item.second.get<std::string>("prompt");
        chat_history.emplace_back(role, prompt);
    }

    // Convert chat history to string
    std::string chat_history_str = chat_history_to_string(chat_history);

    std::string path;
    std::string prompt_template = model_data(model_name)["promptTemplate"].get<std::string>();

    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        path = DatabaseManager::get_path_by_model_name(db, model_name);
        sqlite3_close(db);
    }

    if (stream) {
        send_streaming_response(socket, prompt_template, path, chat_history_str, keep_alive, req.version());
    } else {
        send_non_streaming_response(socket, prompt_template, path, chat_history_str, keep_alive, req.version());
    }
}

// Function to handle HTTP requests
void handle_request(std::shared_ptr<tcp::socket> socket, beast::flat_buffer buffer) {
    try {
        // Read the request
        http::request<http::dynamic_body> req;
        http::read(*socket, buffer, req);

        // Set Keep-Alive
        bool keep_alive = req.keep_alive();

        if (req.method() == http::verb::options) {
            http::response<http::empty_body> res{http::status::no_content, req.version()};
            res.set(http::field::server, "Beast");
            res.set(http::field::access_control_allow_origin, "*");
            res.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
            res.set(http::field::access_control_allow_headers, "Content-Type");
            res.keep_alive(req.keep_alive());

            http::write(*socket, res);
            return;
        }

        if (req.method() == http::verb::post && req.target() == "/api/generate") {
            handle_generate_request(socket, req, keep_alive);
        } else if (req.method() == http::verb::post && req.target() == "/api/chat") {
            handle_chat_request(socket, req, keep_alive);
        } else {
            send_bad_request(socket, keep_alive, req.version());
        }

        if (!keep_alive) {
            socket->shutdown(tcp::socket::shutdown_send);
        }

    } catch (const std::exception &e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
        send_internal_server_error(socket);
    }
}


// Function to start the web server
std::string handle_server() {
    try {
        std::string address = "0.0.0.0";
        unsigned short port = 8080;

        net::io_context ioc;
        tcp::acceptor acceptor{ioc, {net::ip::make_address(address), port}};

        std::cout << "Server started on " << address << ":" << port << std::endl;

        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> thread_pool;

        for (int i = 0; i < num_threads; ++i) {
            thread_pool.emplace_back([&ioc]() { ioc.run(); });
        }

        for (;;) {
            auto socket = std::make_shared<tcp::socket>(ioc);
            acceptor.accept(*socket);

            std::cout << "New connection accepted from " << socket->remote_endpoint() << std::endl;

            beast::flat_buffer buffer;

            std::thread{[socket, buffer]() mutable {
                try {
                    handle_request(socket, std::move(buffer));
                } catch (const std::exception &e) {
                    std::cerr << "Error in request handler thread: " << e.what() << std::endl;
                }
            }}.detach();
        }

        for (auto &thread: thread_pool) {
            thread.join();
        }

    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return "Error: " + std::string(e.what());
    }

    return "Server stopped";
}
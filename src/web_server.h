//
// Created by Amir Kouhkan on 8/8/24.
//

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "nlohmann/json.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

std::string chat_history_to_string(const std::vector<std::pair<std::string, std::string>> &chat_history);

void send_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                             const std::string &path, const std::string &prompt, bool keep_alive, int version);

void send_non_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                                 const std::string &path, const std::string &prompt, bool keep_alive, int version);

void send_bad_request(std::shared_ptr<tcp::socket> socket, bool keep_alive, int version);

void send_internal_server_error(std::shared_ptr<tcp::socket> socket);

void handle_generate_request(std::shared_ptr<tcp::socket> socket, const http::request<http::dynamic_body> &req,
                             bool keep_alive);

void handle_chat_request(std::shared_ptr<tcp::socket> socket, const http::request<http::dynamic_body> &req,
                         bool keep_alive);

void handle_request(std::shared_ptr<tcp::socket> socket, beast::flat_buffer buffer);

std::string handle_server();
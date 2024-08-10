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

std::string handle_api_generate(http::request<http::string_body> const &req, http::response<http::string_body> &res);

//std::string handle_streaming_request(http::request<http::string_body> const &req, http::response<http::string_body> &res,
//                              tcp::socket &socket);

std::string handle_request(tcp::socket &socket, beast::flat_buffer &buffer);

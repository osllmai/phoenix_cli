#pragma once

#include <string>
#include <vector>

#include "commands_list.h"
#include "chat_manager.h"
#include "database_manager.h"
#include "directory_manager.h"
#include "download_model.h"
#include "models_list.h"
#include "chat.h"

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


void handle_request(std::shared_ptr<tcp::socket> socket, beast::flat_buffer buffer);

std::string chat_history_to_string(const std::vector<std::pair<std::string, std::string>> &chat_history);

void send_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                             const std::string &path, const std::string &prompt, bool keep_alive, int version);

void send_non_streaming_response(std::shared_ptr<tcp::socket> socket, const std::string &prompt_template,
                                 const std::string &path, const std::string &prompt, bool keep_alive, int version);

void send_bad_request(std::shared_ptr<tcp::socket> socket, bool keep_alive, int version);

void send_internal_server_error(std::shared_ptr<tcp::socket> socket);

void handle_generate_request(std::shared_ptr<tcp::socket> socket, const http::request <http::dynamic_body> &req,
                             bool keep_alive);

void handle_openai_request(std::shared_ptr<tcp::socket> socket, const http::request <http::dynamic_body> &req,
                             bool keep_alive);

void handle_chat_request(std::shared_ptr<tcp::socket> socket, const http::request <http::dynamic_body> &req,
                         bool keep_alive);

std::string handle_server();

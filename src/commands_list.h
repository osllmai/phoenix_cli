#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>
#include <vector>
#include "sqlite3.h"


void show_commands(int argc, char **argv);
void handle_run_command(const std::string &model_name);


class PhoenixCommandsList {
public:
    using json = nlohmann::json;
    PhoenixCommandsList();
    void run_command(const std::string &command, const std::vector<std::string> &args);
private:
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;
    std::unordered_map<std::string, CommandHandler> commands_;

    void print_help();
    void start_server(const std::vector<std::string> &args);
    void handle_embedding(const std::vector<std::string> &args);
    void handle_openai(const std::vector<std::string> &args);
    void handle_list(const std::vector<std::string> &args);
    void handle_run(const std::vector<std::string> &args);
    void handle_pull(const std::vector<std::string> &args);
    void handle_remove(const std::vector<std::string> &args);
    void handle_history(const std::vector<std::string> &args);
    void handle_exec(const std::vector<std::string> &args);
    void handle_show(const std::vector<std::string> &args);


};
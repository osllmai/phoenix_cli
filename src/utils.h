#pragma once

#include "header.h"
#include "parse_json.h"

// Need this for Windows colors
#ifdef _WIN32
#include <windows.h>
#endif

// Function to check if a string contains a substring
bool containsSubstring(const std::string &str, const std::string &substr);

// Function to check AVX support at startup
void check_avx_support_at_startup();

//////////////////////////////////////////////////////////////////////////
////////////                 SIGNAL HANDLING                  ////////////
//////////////////////////////////////////////////////////////////////////

extern volatile sig_atomic_t sighup_received;

void handle_sighup(int signal);

#ifdef _WIN32
BOOL WINAPI console_ctrl_handler(DWORD ctrl_type);
#endif

//////////////////////////////////////////////////////////////////////////
////////////                /SIGNAL HANDLING                  ////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
////////////            READ PROMPT TEMPLATE FILE             ////////////
//////////////////////////////////////////////////////////////////////////

// Function to read the prompt template file
std::tuple<std::string, std::string, std::string> read_prompt_template_file(const std::string& file_path);

//////////////////////////////////////////////////////////////////////////
////////////           /READ PROMPT TEMPLATE FILE             ////////////
//////////////////////////////////////////////////////////////////////////

// Function to save the chat log
void save_chat_log(std::string save_log, std::string prompt, std::string answer);

// Function to read the chat log
std::string read_chat_log(std::string load_log);

// Function to get the directory part of a pathname
std::string pathname_directory(const std::string &pathname);

// Function to set the console color
void set_console_color(ConsoleState &con_st, ConsoleColor color);

// Function to generate a random prompt
std::string random_prompt(int32_t seed);

// Function to print the version information
void print_version();

// Function to print the usage information
void print_usage(int argc, char** argv, const chatParams& params);

// Function to parse command-line parameters
bool parse_params(int argc, char** argv, chatParams& params);

#pragma once

#include "header.h"

#include <string>
#include <map>
#include <regex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>

// Helper function to convert string to bool
bool stob(const std::string& str);

// Function to read the contents of a file
std::string readFile(const std::string& filename);

// Function to parse a JSON string into a map
std::map<std::string, std::string> parse_json_string(const std::string& jsonString);

// Function to remove quotes from a string
std::string removeQuotes(const std::string& input);

// Function to get parameters from a JSON file and populate chatParams
void get_params_from_json(chatParams& params);

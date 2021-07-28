#include "utils.hpp"

// Yoinked from: https://stackoverflow.com/a/46931770
std::vector<std::string> utils::split(std::string string, std::string delimiter)
{
    size_t pos_start = 0;
    size_t pos_end = 0;
    size_t delim_len = delimiter.length();

    std::string token;
    std::vector<std::string> result;

    while ((pos_end = string.find(delimiter, pos_start)) != std::string::npos) {
        token = string.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        result.push_back(token);
    }

    result.push_back(string.substr(pos_start));

    return result;
}

// Splits a string at spaces and discards any empty strings. The name is misleading!
std::vector<std::string> utils::split_whitespace(std::string string)
{
    std::string delimiter = " ";

    size_t pos_start = 0;
    size_t pos_end = 0;
    size_t delim_len = delimiter.length();

    std::string token;
    std::vector<std::string> result;

    while ((pos_end = string.find(delimiter, pos_start)) != std::string::npos) {
        token = string.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;

        if (token.length() == 0) {
            continue;
        }

        result.push_back(token);
    }

    result.push_back(string.substr(pos_start));

    return result;
}

#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <vector>

namespace utils {
std::vector<std::string> split(std::string string, std::string delimiter);
std::vector<std::string> split_whitespace(std::string string);
}
#endif

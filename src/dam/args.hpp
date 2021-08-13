#ifndef DAM_ARGS_HPP
#define DAM_ARGS_HPP
#include <string>
#include <vector>

namespace dam {
typedef std::vector<std::string> Args;

namespace args {
Args create(int argc, char* argv[]);
}
}
#endif

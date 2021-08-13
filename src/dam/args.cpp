#include "args.hpp"

dam::Args dam::args::create(int argc, char* argv[])
{
    dam::Args result;

    for (int i = 0; i < argc; ++i) {
        result.push_back(argv[i]);
    }

    return result;
}

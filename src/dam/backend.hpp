#ifndef DAM_BACKEND_HPP
#define DAM_BACKEND_HPP
#include "app.hpp"

namespace dam {
class Backend;
}

class dam::Backend {
public:
    virtual void run(dam::App& app) = 0;
};
#endif

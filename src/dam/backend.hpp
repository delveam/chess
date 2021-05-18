#ifndef DAM_BACKEND_HPP
#define DAM_BACKEND_HPP
#include "app.hpp"
#include "app_config.hpp"

namespace dam {
class Backend;
}

class dam::Backend {
public:
    virtual void run(AppConfig& config, App& app) = 0;
};
#endif

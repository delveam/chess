#ifndef DAM_APP_HPP
#define DAM_APP_HPP
#include "context.hpp"

namespace dam {
class App;
}

class dam::App {
public:
    App();
    bool loop;
    virtual void initialize(Context& ctx) = 0;
    virtual void update(Context& ctx) = 0;
    virtual void draw(Context& ctx) = 0;
    virtual void destroy(Context& ctx) = 0;
};
#endif

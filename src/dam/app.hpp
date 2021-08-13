#ifndef DAM_APP_HPP
#define DAM_APP_HPP
#include "app_config.hpp"
#include "args.hpp"
#include "context.hpp"
#include "event_type.hpp"

namespace dam {
class App;
}

class dam::App {
public:
    App() = default;
    App(Args args) :
        m_args(args)
    {
    }

    const Args& args() const
    {
        return m_args;
    }

    bool loop { true };

    virtual void initialize(Context& ctx) = 0;
    virtual void update(Context& ctx) = 0;
    virtual void event(Context& ctx, EventType event) = 0;
    virtual void draw(Context& ctx) = 0;
    virtual void destroy(Context& ctx) = 0;
private:
    dam::Args m_args;
};
#endif

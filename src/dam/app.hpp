#ifndef APP_HPP
#define APP_HPP
namespace dam {
class App;
}

class dam::App {
public:
    App();
    bool loop;
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void destroy() = 0;
};
#endif

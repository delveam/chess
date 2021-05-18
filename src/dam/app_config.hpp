#ifndef DAM_APP_CONFIG_HPP
#define DAM_APP_CONFIG_HPP
#include <string>

namespace dam {
class AppConfig;
}

class dam::AppConfig {
public:
    AppConfig();
    std::string title;
    // bool vsync;
    bool resizable_window;
    bool mouse_visibility;

    AppConfig& set_title(std::string title);
    // AppConfig& enable_vsync();
    AppConfig& enable_resizable_window();
    AppConfig& disable_mouse_visibility();
};
#endif

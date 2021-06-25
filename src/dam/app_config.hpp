#ifndef DAM_APP_CONFIG_HPP
#define DAM_APP_CONFIG_HPP
#include <string>

namespace dam {
class AppConfig;
}

class dam::AppConfig {
public:
    AppConfig();

    std::string title() const
    {
        return m_title;
    };
    bool resizable_window() const
    {
        return m_resizable_window;
    };
    bool mouse_visibility() const
    {
        return m_mouse_visibility;
    };

    AppConfig& set_title(std::string title);
    AppConfig& enable_resizable_window();
    AppConfig& disable_mouse_visibility();
private:
    std::string m_title;
    bool m_resizable_window;
    bool m_mouse_visibility;
};
#endif

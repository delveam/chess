#include "app_config.hpp"

dam::AppConfig& dam::AppConfig::set_title(std::string title)
{
    m_title = title;

    return *this;
}

dam::AppConfig& dam::AppConfig::enable_resizable_window()
{
    m_resizable_window = true;

    return *this;
}

dam::AppConfig& dam::AppConfig::disable_mouse_visibility()
{
    m_mouse_visibility = false;

    return *this;
}

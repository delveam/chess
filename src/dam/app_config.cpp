#include "app_config.hpp"

dam::AppConfig::AppConfig()
{
    title = "My App";
    // vsync = false;
    resizable_window = false;
    mouse_visibility = true;
}

dam::AppConfig& dam::AppConfig::set_title(std::string title)
{
    this->title = title;

    return *this;
}

// dam::AppConfig& dam::AppConfig::enable_vsync()
// {
//     vsync = true;

//     return *this;
// }

dam::AppConfig& dam::AppConfig::enable_resizable_window()
{
    resizable_window = true;

    return *this;
}

dam::AppConfig& dam::AppConfig::disable_mouse_visibility()
{
    mouse_visibility = false;

    return *this;
}

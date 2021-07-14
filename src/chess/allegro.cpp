#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include "allegro.hpp"

void Allegro::run(dam::AppConfig& config, dam::App& app)
{
    if(!al_init()) {
        std::cout << "Couldn't initialize allegro.\n";
        return;
    }

    if (!al_init_image_addon()) {
        std::cout << "Couldn't initialize image addon.\n";
        return;
    }

    if(!al_init_font_addon()) {
        std::cout << "Couldn't initialize font addon.\n";
        return;
    }

    if(!al_init_ttf_addon()) {
        std::cout << "Couldn't initialize ttf font addon.\n";
        return;
    }

    if(!al_install_keyboard()) {
        std::cout << "Couldn't initialize keyboard.\n";
        return;
    }

    if(!al_install_mouse()) {
        std::cout << "Couldn't initialize mouse.\n";
        return;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue) {
        std::cout << "Couldn't initialize queue.\n";
        return;
    }

    auto default_window_width = 640;
    auto default_window_height = 360;

    ALLEGRO_CONFIG* user_config = al_load_config_file("config.cfg");
    if (user_config != NULL) {
        auto window_width = al_get_config_value(user_config, "", "window_width");
        if (window_width != NULL) {
            default_window_width = std::stoi(window_width);
        }

        auto window_height = al_get_config_value(user_config, "", "window_height");
        if (window_width != NULL) {
            default_window_height = std::stoi(window_height);
        }
    }

    auto display_flags = 0;
    display_flags = config.resizable_window() ? display_flags | ALLEGRO_RESIZABLE : display_flags;
    al_set_new_display_flags(display_flags);
    ALLEGRO_DISPLAY* display = al_create_display(default_window_width, default_window_height);
    if(!display) {
        std::cout << "Couldn't initialize display.\n";
        return;
    }

    // NOTE: Without the following line of code the app may not be centered correctly upon startup.
    al_acknowledge_resize(display);

    al_set_window_title(display, config.title().c_str());

    if (!config.mouse_visibility()) {
        al_hide_mouse_cursor(display);
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    ALLEGRO_EVENT event;
    auto should_close = false;

    // TODO: Some AppConfig options should be transfered to Context (e.g. vsync, mouse_visiblity, etc.)
    auto ctx = dam::Context();
    ctx.display = display;

    ALLEGRO_MONITOR_INFO monitor_info;
    if (al_get_monitor_info(0, &monitor_info)) {
        ctx.display_width = monitor_info.x2 - monitor_info.x1;
        ctx.display_height = monitor_info.y2 - monitor_info.y1;
    }
    else {
        // It looks like we cannot determine the display's dimensions.
        // Until we can figure out a workaround, just set the display's dimensions to
        // the initial window's dimensions.
        ctx.display_width = default_window_width;
        ctx.display_height= default_window_height;
    }

    ctx.keyboard = dam::input::SmartKeyboard();
    ctx.mouse = dam::input::SmartMouse();

    app.initialize(ctx);

    // This might be a little egregious for chess? I don't know...
    auto target = 1.0 / 250.0;
    auto accumulator = 0.0;
    auto max_frame_skip = 25;
    auto max_delta_time = max_frame_skip * target;

    auto previous_time = al_get_time();

    ctx.total_time = 0;
    // The app updates using a fixed time-step.
    ctx.delta_time = target;
    ctx.alpha = 0;

    while (!should_close) {
        while(al_get_next_event(queue, &event)) {
            switch(event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // TODO: Should we also send this event to the app?
                should_close = true;
                break;

            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                app.event(ctx, dam::EventType::WindowResize);
                al_acknowledge_resize(display);
                break;
            }
        }

        auto current_time = al_get_time();
        auto delta_time = current_time - previous_time;

        // Set a maximum delta time in order to avoid a "Spiral of Doom."
        if (delta_time > max_delta_time) {
            delta_time = max_delta_time;
        }

        previous_time = current_time;

        accumulator += delta_time;

        while (accumulator >= target) {
            ctx.keyboard.update();
            ctx.mouse.update();
            app.update(ctx);
            if (!app.loop) {
                should_close = true;
                break;
            }

            accumulator -= target;
            ctx.total_time += target;
        }

        ctx.alpha = accumulator / target;

        app.draw(ctx);

        al_flip_display();
    }

    app.destroy(ctx);

    al_destroy_config(user_config);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
}

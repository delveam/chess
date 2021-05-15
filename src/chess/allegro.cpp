#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <iostream>
#include "allegro.hpp"

void Allegro::run(dam::App& app)
{
    if(!al_init()) {
        std::cout << "Couldn't initialize allegro.\n";
        return;
    }

    if(!al_install_keyboard()) {
        std::cout << "Couldn't initialize keyboard.\n";
        return;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    if(!timer) {
        std::cout << "Couldn't initialize timer.\n";
        return;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue) {
        std::cout << "Couldn't initialize queue.\n";
        return;
    }

    auto default_window_width = 640;
    auto default_window_height = 360;

    ALLEGRO_CONFIG* config = al_load_config_file("config.cfg");
    if (config != NULL) {
        auto window_width = al_get_config_value(config, "", "window_width");
        if (window_width != NULL) {
            default_window_width = std::stoi(window_width);
        }

        auto window_height = al_get_config_value(config, "", "window_height");
        if (window_width != NULL) {
            default_window_height = std::stoi(window_height);
        }
    }

    ALLEGRO_DISPLAY* disp = al_create_display(default_window_width, default_window_height);
    if(!disp) {
        std::cout << "Couldn't initialize display.\n";
        return;
    }

    if (!al_init_image_addon()) {
        std::cout << "Couldn't initialize image addon.\n";
        return;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_EVENT event;
    auto should_close = false;
    auto redraw = true;

    auto ctx = dam::Context();
    ctx.display = disp;

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

    app.initialize(ctx);

    al_start_timer(timer);
    while(!should_close) {
        al_wait_for_event(queue, &event);

        switch(event.type) {
        case ALLEGRO_EVENT_TIMER:
            ctx.keyboard.update();
            app.update(ctx);
            redraw = true;
            if (!app.loop) {
                should_close = true;
            }
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            should_close = true;
            break;
        }

        if(redraw && al_is_event_queue_empty(queue)) {
            app.draw(ctx);
            al_flip_display();
            redraw = false;
        }
    }

    app.destroy(ctx);

    al_destroy_config(config);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}

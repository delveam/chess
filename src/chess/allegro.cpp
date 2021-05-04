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

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
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
    bool should_close = false;
    bool redraw = true;

    auto ctx = dam::Context();
    ctx.display = disp;

    ALLEGRO_MONITOR_INFO* monitor_info;
    al_get_monitor_info(0, monitor_info);

    ctx.display_width = monitor_info->x2 - monitor_info->x1;
    ctx.display_height= monitor_info->y2 - monitor_info->y1;
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

    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}

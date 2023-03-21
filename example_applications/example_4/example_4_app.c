
#include "example_4_app.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>

#include <input/input.h>

static void example_4_app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    Example4App* app = ctx;

    canvas_clear(canvas);

    DrawMode mode = app->draw_mode;
    if(mode == DRAW_ONLY_PICTURES || mode == DRAW_ALL)
        canvas_draw_icon(canvas, 0, 29, &I_amperka_ru_logo_128x35px);
    if(mode == DRAW_ONLY_TEXT || mode == DRAW_ALL) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 8, "This is an example app!");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(
            canvas,
            127,
            15,
            AlignRight,
            AlignTop,
            "Some long long long long \n aligned multiline text");
    }
}

static void example_4_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

Example4App* example_4_app_alloc() {
    Example4App* app = malloc(sizeof(Example4App));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, example_4_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, example_4_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void example_4_app_free(Example4App* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}

int32_t example_4_app(void* p) {
    UNUSED(p);
    Example4App* app = example_4_app_alloc();

    InputEvent event;

    while(1) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                if(event.key == InputKeyBack) break;
            } else if(event.type == InputTypeLong) {
                DrawMode mode = app->draw_mode;
                if(event.key == InputKeyLeft)
                    app->draw_mode = (mode - 1 + TOTAL_DRAW_MODES) % TOTAL_DRAW_MODES;
                else if(event.key == InputKeyRight)
                    app->draw_mode = (mode + 1) % TOTAL_DRAW_MODES;

                view_port_update(app->view_port);
            }
        }
    }

    example_4_app_free(app);
    return 0;
}

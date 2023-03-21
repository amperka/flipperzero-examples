
#include "example_6_app.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

static void example_6_app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    Example6App* app = ctx;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 32, 17, AlignCenter, AlignTop, "Output PA7:");
    elements_multiline_text_aligned(canvas, 96, 17, AlignCenter, AlignTop, "Input PA6:");

    canvas_set_font(canvas, FontBigNumbers);
    elements_multiline_text_aligned(
        canvas, 32, 32, AlignCenter, AlignTop, app->output_value ? "1" : "0");
    elements_multiline_text_aligned(
        canvas, 96, 32, AlignCenter, AlignTop, app->input_value ? "1" : "0");
}

static void example_6_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

Example6App* example_6_app_alloc() {
    Example6App* app = malloc(sizeof(Example6App));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, example_6_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, example_6_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->input_pin = &gpio_ext_pa6;
    app->output_pin = &gpio_ext_pa7;

    furi_hal_gpio_init(app->input_pin, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(app->output_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    return app;
}

void example_6_app_free(Example6App* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}

int32_t example_6_app(void* p) {
    UNUSED(p);
    Example6App* app = example_6_app_alloc();

    InputEvent event;

    while(1) {
        furi_hal_gpio_write(app->output_pin, app->output_value);
        app->input_value = furi_hal_gpio_read(app->input_pin);

        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.key == InputKeyBack) {
                if(event.type == InputTypePress) break;

            } else if(event.key == InputKeyOk) {
                if(event.type == InputTypePress)
                    app->output_value = true;
                else if(event.type == InputTypeRelease)
                    app->output_value = false;
            }
        }
    }

    example_6_app_free(app);
    return 0;
}

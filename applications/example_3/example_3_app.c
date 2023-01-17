
#include "example_3_app.h"

#include <furi.h>
#include <gui/gui.h>

#include <gui/elements.h>

static void example_3_app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);

    canvas_draw_icon(canvas, 0, 29, &I_amperka_ru_logo_128x35px);

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

Example3App* example_3_app_alloc() {
    Example3App* app = malloc(sizeof(Example3App));

    app->view_port = view_port_alloc();

    view_port_draw_callback_set(app->view_port, example_3_app_draw_callback, NULL);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void example_3_app_free(Example3App* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);
}

int32_t example_3_app(void* p) {
    UNUSED(p);
    Example3App* app = example_3_app_alloc();

    furi_delay_ms(10000);

    example_3_app_free(app);
    return 0;
}

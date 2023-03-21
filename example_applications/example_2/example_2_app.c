
#include "example_2_app.h"

#include <furi.h>
#include <gui/gui.h>

Example2App* example_2_app_alloc() {
    Example2App* app = malloc(sizeof(Example2App));

    app->view_port = view_port_alloc();

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void example_2_app_free(Example2App* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);
}

int32_t example_2_app(void* p) {
    UNUSED(p);
    Example2App* app = example_2_app_alloc();

    furi_delay_ms(10000);

    example_2_app_free(app);
    return 0;
}

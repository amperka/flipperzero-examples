#include "wait_connection.h"

#include <assets_icons.h>

#include <gui/elements.h>

struct WaitConnection {
    View* view;
};

static void wait_connection_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_draw_icon(canvas, 0, 0, &I_BLE_Pairing_128x64);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 50, 4, AlignCenter, AlignTop, "Waiting for connection\nfrom HM-10 module!");
}

static bool wait_connection_input_callback(InputEvent* event, void* ctx) {
    UNUSED(ctx);
    UNUSED(event);
    return 0;
}

WaitConnection* wait_connection_alloc() {
    WaitConnection* wait_connection = malloc(sizeof(WaitConnection));

    wait_connection->view = view_alloc();
    view_set_context(wait_connection->view, wait_connection);
    view_set_draw_callback(wait_connection->view, wait_connection_draw_callback);
    view_set_input_callback(wait_connection->view, wait_connection_input_callback);

    return wait_connection;
}

void wait_connection_free(WaitConnection* wait_connection) {
    furi_assert(wait_connection);
    view_free(wait_connection->view);
    free(wait_connection);
}

View* wait_connection_get_view(WaitConnection* wait_connection) {
    furi_assert(wait_connection);
    return wait_connection->view;
}


#include <furi.h>
#include <furi_hal_bt.h>
#include <bt/bt_service/bt.h>
#include <input/input.h>

#include "bt_serial_example_app.h"

#define TAG "BtSerialExampleApp"

static uint16_t bt_serial_event_callback(SerialServiceEvent event, void* ctx) {
    UNUSED(ctx);

    if(event.event == SerialServiceEventTypeDataReceived) {
        FuriString* data_str;
        data_str = furi_string_alloc();
        for(size_t i = 0; i < event.data.size; i++) {
            furi_string_cat_printf(data_str, "0x%02X ", event.data.buffer[i]);
        }

        FURI_LOG_I(TAG, "Got data: %s", furi_string_get_cstr(data_str));
        furi_string_free(data_str);
    }

    return 0;
}

static void bt_serial_example_app_connection_status_changed_callback(BtStatus status, void* ctx) {
    furi_assert(ctx);
    BtSerialExampleApp* app = ctx;

    if(status == BtStatusConnected) {
        FURI_LOG_I(TAG, "Starting Timer!");
        furi_timer_start(app->timer, 1000);
    }
}

static void bt_serial_example_app_timer_callback(void* ctx) {
    furi_assert(ctx);
    BtSerialExampleApp* app = ctx;

    FURI_LOG_I(TAG, "Sending byte: %d!", app->byte_to_send);
    furi_hal_bt_serial_tx(&(app->byte_to_send), 1);
    app->byte_to_send++;
}

static void bt_serial_example_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

BtSerialExampleApp* bt_serial_example_app_alloc() {
    BtSerialExampleApp* app = malloc(sizeof(BtSerialExampleApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();

    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_input_callback_set(
        app->view_port, bt_serial_example_app_input_callback, app->event_queue);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->byte_to_send = 0;

    app->bt = furi_record_open(RECORD_BT);
    app->timer =
        furi_timer_alloc(bt_serial_example_app_timer_callback, FuriTimerTypePeriodic, app);

    return app;
}

void bt_serial_example_app_free(BtSerialExampleApp* app) {
    furi_assert(app);

    furi_record_close(RECORD_BT);
    app->bt = NULL;

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    app->view_port = NULL;

    furi_message_queue_free(app->event_queue);
    app->event_queue = NULL;

    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;

    free(app);
}

int32_t bt_serial_example_app(void* p) {
    UNUSED(p);

    BtSerialExampleApp* app = bt_serial_example_app_alloc();

    bt_disconnect(app->bt);
    bt_set_profile(app->bt, BtProfileArduino);
    furi_hal_bt_serial_set_event_callback(128, bt_serial_event_callback, NULL);
    bt_set_status_changed_callback(
        app->bt, bt_serial_example_app_connection_status_changed_callback, app);
    furi_hal_bt_start_advertising();

    InputEvent event;

    while(1) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                if(event.key == InputKeyBack) break;
            }
        }
    }

    furi_hal_bt_serial_set_event_callback(0, NULL, NULL);
    bt_disconnect(app->bt);

    bt_serial_example_app_free(app);

    return 0;
}

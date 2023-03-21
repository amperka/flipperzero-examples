
#include "bluetooth_teleop_app.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <bt/bt_service/bt.h>

static bool bluetooth_teleop_app_back_event_callback(void* ctx) {
    furi_assert(ctx);
    BluetoothTeleopApp* app = ctx;
    if(!app->connected)
        notification_message(app->notifications, &sequence_blink_stop);
    else
        notification_message(app->notifications, &sequence_reset_rgb);

    view_dispatcher_stop(app->view_dispatcher);
    furi_timer_stop(app->timer);
    return 0;
}

static void bluetooth_teleop_connection_status_changed_callback(BtStatus status, void* ctx) {
    furi_assert(ctx);
    BluetoothTeleopApp* app = ctx;

    if(status == BtStatusConnected) {
        app->connected = true;
        notification_message(app->notifications, &sequence_blink_stop);
        notification_message(app->notifications, &sequence_set_blue_255);
        view_dispatcher_switch_to_view(app->view_dispatcher, BluetoothTeleopAppViewControl);
        furi_timer_start(app->timer, 150);
    } else if(status != BtStatusConnected && app->connected) {
        app->connected = false;
        notification_message(app->notifications, &sequence_blink_start_yellow);
        view_dispatcher_switch_to_view(app->view_dispatcher, BluetoothTeleopAppViewWaitConnection);
        furi_timer_stop(app->timer);
    }
}

static void bluetooth_teleop_timer_callback(void* ctx) {
    furi_assert(ctx);
    BluetoothTeleopApp* app = ctx;
    uint8_t data = control_get_cmd(app->control);
    furi_hal_bt_serial_tx(&data, 1);
}

BluetoothTeleopApp* bluetooth_teleop_app_alloc() {
    BluetoothTeleopApp* app = malloc(sizeof(BluetoothTeleopApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, bluetooth_teleop_app_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->wait_connection = wait_connection_alloc();
    app->control = control_alloc();

    app->timer = furi_timer_alloc(bluetooth_teleop_timer_callback, FuriTimerTypePeriodic, app);

    view_dispatcher_add_view(
        app->view_dispatcher,
        BluetoothTeleopAppViewWaitConnection,
        wait_connection_get_view(app->wait_connection));
    view_dispatcher_add_view(
        app->view_dispatcher, BluetoothTeleopAppViewControl, control_get_view(app->control));

    app->bt = furi_record_open(RECORD_BT);

    view_dispatcher_switch_to_view(app->view_dispatcher, BluetoothTeleopAppViewWaitConnection);
    notification_message(app->notifications, &sequence_blink_start_yellow);

    return app;
}

void bluetooth_teleop_app_free(BluetoothTeleopApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, BluetoothTeleopAppViewWaitConnection);
    view_dispatcher_remove_view(app->view_dispatcher, BluetoothTeleopAppViewControl);

    wait_connection_free(app->wait_connection);
    control_free(app->control);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_BT);
    app->bt = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;

    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);

    free(app);
}

int32_t bluetooth_teleop_app(void* p) {
    UNUSED(p);
    BluetoothTeleopApp* app = bluetooth_teleop_app_alloc();

    bt_disconnect(app->bt);
    bt_set_profile(app->bt, BtProfileArduino);
    bt_set_status_changed_callback(
        app->bt, bluetooth_teleop_connection_status_changed_callback, app);
    furi_hal_bt_start_advertising();

    view_dispatcher_run(app->view_dispatcher);

    bt_set_status_changed_callback(app->bt, NULL, NULL);
    bt_disconnect(app->bt);

    bluetooth_teleop_app_free(app);
    return 0;
}

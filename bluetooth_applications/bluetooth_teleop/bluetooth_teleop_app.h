#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <bt/bt_service/bt.h>
#include <gui/view_dispatcher.h>
#include <notification/notification_messages.h>

#include "views/wait_connection.h"
#include "views/control.h"

struct BluetoothTeleopApp {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;

    Bt* bt;
    WaitConnection* wait_connection;
    Control* control;

    FuriTimer* timer;
    bool connected;
};

typedef enum {
    BluetoothTeleopAppViewWaitConnection,
    BluetoothTeleopAppViewControl,
} BluetoothTeleopAppView;

typedef struct BluetoothTeleopApp BluetoothTeleopApp;

#pragma once

#include <furi.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>

struct BtSerialExampleApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Bt* bt;
    FuriTimer* timer;

    uint8_t byte_to_send;
};

typedef struct BtSerialExampleApp BtSerialExampleApp;
#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

struct Example6App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    const GpioPin* input_pin;
    const GpioPin* output_pin;

    bool input_value;
    bool output_value;
};

typedef struct Example6App Example6App;

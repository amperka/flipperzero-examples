#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

struct ADCExample {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    const GpioPin* adc_pin;
    uint32_t adc_value;
};

typedef struct ADCExample ADCExample;

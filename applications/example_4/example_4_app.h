#pragma once

#include <furi.h>
#include <gui/gui.h>

#include "example_4_icons.h"

typedef enum {
    DRAW_ALL,
    DRAW_ONLY_TEXT,
    DRAW_ONLY_PICTURES,
    TOTAL_DRAW_MODES = 3,
} DrawMode;

struct Example4App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    DrawMode draw_mode;
};

typedef struct Example4App Example4App;

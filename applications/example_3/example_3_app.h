#pragma once

#include <furi.h>
#include <gui/gui.h>

#include "example_3_icons.h"

struct Example3App {
    Gui* gui;
    ViewPort* view_port;
};

typedef struct Example3App Example3App;

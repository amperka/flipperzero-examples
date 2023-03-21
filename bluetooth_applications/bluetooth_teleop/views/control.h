#pragma once

#include <gui/view.h>

typedef struct Control Control;

Control* control_alloc();
void control_free(Control* control);
View* control_get_view(Control* control);

uint8_t control_get_cmd(Control* control);

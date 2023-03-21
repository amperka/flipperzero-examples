#pragma once

#include <gui/view.h>

typedef struct WaitConnection WaitConnection;

WaitConnection* wait_connection_alloc();
void wait_connection_free(WaitConnection* wait_connection);
View* wait_connection_get_view(WaitConnection* wait_connection);

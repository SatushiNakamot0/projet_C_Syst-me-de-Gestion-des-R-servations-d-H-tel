#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include "ui_state.h"
#include <stdbool.h>

/**
 * Shows the login screen.
 * Returns true if login is successful, false if user cancelled/exited (ESC).
 */
bool show_login_screen(UIContext *ctx);

#endif /* UI_LOGIN_H */

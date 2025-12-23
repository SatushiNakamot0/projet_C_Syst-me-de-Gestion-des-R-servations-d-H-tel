#ifndef UI_H
#define UI_H

#include "ui_state.h"

/* ============================================================================
 * MAIN UI INTERFACE
 * ============================================================================
 * Public API for the TUI system. This is the main entry point.
 * ============================================================================ */

/* Initialize the UI system */
bool ui_init(UIContext *ctx);

/* Run the main UI loop */
void ui_run(UIContext *ctx);

/* Cleanup and shutdown */
void ui_cleanup(UIContext *ctx);

/* Force a full redraw */
void ui_redraw(UIContext *ctx);

#endif /* UI_H */


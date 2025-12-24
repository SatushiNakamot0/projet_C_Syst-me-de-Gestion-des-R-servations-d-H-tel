#ifndef UI_INPUT_H
#define UI_INPUT_H

#include "ui_state.h"

/* ============================================================================
 * UI INPUT HANDLING
 * ============================================================================
 * Keyboard input processing and navigation logic.
 * Handles all keyboard events and converts them to navigation actions.
 * ============================================================================ */

/* Initialize input system */
void ui_input_init(void);

/* Process a single keypress and return navigation direction */
NavDirection ui_input_process_key(UIContext *ctx, int key);

/* Handle special keys */
bool ui_input_is_arrow_key(int key);
NavDirection ui_input_arrow_to_direction(int key);

/* Process input for forms */
void ui_input_process_form_input(UIContext *ctx, int key);

/* Get non-blocking input */
int ui_input_get_key(void);

/* Reads string input safely with visual feedback
 * Returns: 1 on Enter, 0 on ESC/Cancel
 */
int ui_read_line(WINDOW *win, int y, int x, char *buffer, int max_len);

#endif /* UI_INPUT_H */

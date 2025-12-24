#ifndef UI_FORM_H
#define UI_FORM_H

#include "ui_state.h"

// Form handling return values
#define FORM_CONTINUE 0     // Continue processing input
#define FORM_SUBMIT   1     // Form should be submitted
#define FORM_CANCEL   2     // Form was cancelled

void ui_form_start_input(UIContext *ctx, int mode);
void ui_form_stop_input(UIContext *ctx);
int ui_form_handle_input(UIContext *ctx, int key);  // Returns int instead of void
void ui_form_cleanup(UIContext *ctx);

#endif /* UI_FORM_H */

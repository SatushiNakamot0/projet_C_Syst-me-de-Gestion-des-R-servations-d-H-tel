#ifndef UI_FORM_H
#define UI_FORM_H

#include "ui_state.h"
#include "../include/structures.h"

// Form handling return values
#define FORM_CONTINUE 0 // Continue processing input
#define FORM_SUBMIT 1   // Form should be submitted
#define FORM_CANCEL 2   // Form was cancelled

void ui_form_start_input(UIContext *ctx, int mode);
void ui_form_stop_input(UIContext *ctx);
int ui_form_handle_input(UIContext *ctx, int key); // Returns int instead of void
void ui_form_cleanup(UIContext *ctx);

// Modal forms with proper cleanup and visual separation
int show_edit_client_form(Client *client, Client clients[], int count);
// show_add_reservation_form removed

#endif /* UI_FORM_H */

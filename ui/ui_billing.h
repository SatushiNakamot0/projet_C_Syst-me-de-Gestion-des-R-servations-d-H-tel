#ifndef UI_BILLING_H
#define UI_BILLING_H

#include "ui_state.h"

/**
 * Shows the main billing menu (blocking loop)
 * Allows creating new invoices or viewing history
 */
void show_billing_menu(UIContext *ctx);

#endif /* UI_BILLING_H */

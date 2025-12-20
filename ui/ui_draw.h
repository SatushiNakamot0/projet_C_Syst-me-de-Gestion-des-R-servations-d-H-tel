#ifndef UI_DRAW_H
#define UI_DRAW_H

#include "ui_state.h"
#include "ui_layout.h"

/* ============================================================================
 * UI DRAWING FUNCTIONS
 * ============================================================================
 * All screen drawing functions for each UI state.
 * Each function draws a complete screen layout.
 * ============================================================================ */

/* Layout drawing */
void ui_draw_header(UIContext *ctx, Layout *layout);
void ui_draw_sidebar(UIContext *ctx, Layout *layout);
void ui_draw_footer(UIContext *ctx, Layout *layout);

/* State-specific drawing functions */
void ui_draw_dashboard(UIContext *ctx, Layout *layout);
void ui_draw_clients_list(UIContext *ctx, Layout *layout);
void ui_draw_clients_add(UIContext *ctx, Layout *layout);
void ui_draw_clients_edit(UIContext *ctx, Layout *layout);
void ui_draw_clients_search(UIContext *ctx, Layout *layout);
void ui_draw_rooms_list(UIContext *ctx, Layout *layout);
void ui_draw_rooms_add(UIContext *ctx, Layout *layout);
void ui_draw_reservations_list(UIContext *ctx, Layout *layout);
void ui_draw_reservations_add(UIContext *ctx, Layout *layout);
void ui_draw_billing_list(UIContext *ctx, Layout *layout);
void ui_draw_billing_create(UIContext *ctx, Layout *layout);
void ui_draw_help(UIContext *ctx, Layout *layout);

/* Dialog/Modal drawing */
void ui_draw_dialog(UIContext *ctx);
void ui_draw_status_message(UIContext *ctx, Layout *layout);

#endif /* UI_DRAW_H */


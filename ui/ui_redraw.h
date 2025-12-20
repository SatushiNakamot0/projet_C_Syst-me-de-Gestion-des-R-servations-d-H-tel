#ifndef UI_REDRAW_H
#define UI_REDRAW_H

#include "ui_state.h"
#include "ui_layout.h"

/* ============================================================================
 * OPTIMIZED REDRAW SYSTEM
 * ============================================================================
 * Implements dirty-region tracking and partial redraws to minimize
 * screen flicker and improve performance.
 * ============================================================================ */

/* Dirty region flags - track what needs redrawing */
typedef struct {
    bool header_dirty;
    bool sidebar_dirty;
    bool content_dirty;
    bool footer_dirty;
    bool status_dirty;
    bool dialog_dirty;
    
    /* Full redraw flag - set on state change or resize */
    bool full_redraw;
    
    /* Last drawn state - for comparison */
    UIState last_state;
    int last_term_rows;
    int last_term_cols;
} DirtyFlags;

/* Initialize dirty flags */
void ui_redraw_init(DirtyFlags *flags);

/* Mark regions as dirty */
void ui_redraw_mark_header(DirtyFlags *flags);
void ui_redraw_mark_sidebar(DirtyFlags *flags);
void ui_redraw_mark_content(DirtyFlags *flags);
void ui_redraw_mark_footer(DirtyFlags *flags);
void ui_redraw_mark_status(DirtyFlags *flags);
void ui_redraw_mark_dialog(DirtyFlags *flags);
void ui_redraw_mark_all(DirtyFlags *flags);

/* Check if redraw needed */
bool ui_redraw_needed(const DirtyFlags *flags);

/* Check if specific region needs redraw */
bool ui_redraw_header_needed(const DirtyFlags *flags);
bool ui_redraw_sidebar_needed(const DirtyFlags *flags);
bool ui_redraw_content_needed(const DirtyFlags *flags);
bool ui_redraw_footer_needed(const DirtyFlags *flags);
bool ui_redraw_status_needed(const DirtyFlags *flags);
bool ui_redraw_dialog_needed(const DirtyFlags *flags);

/* Clear dirty flags after redraw */
void ui_redraw_clear(DirtyFlags *flags);
void ui_redraw_clear_region(DirtyFlags *flags, int region);

/* Optimized refresh - batches updates */
void ui_redraw_refresh_batch(void);

/* Check for resize and mark all dirty if needed */
void ui_redraw_check_resize(DirtyFlags *flags, UIContext *ctx);

#endif /* UI_REDRAW_H */


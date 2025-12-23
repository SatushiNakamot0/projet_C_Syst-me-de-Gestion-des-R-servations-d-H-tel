#include "ui_layout.h"
#include <string.h>

/* ============================================================================
 * UI LAYOUT IMPLEMENTATION
 * ============================================================================
 * Calculates layout regions based on terminal size.
 * Ensures minimum sizes and graceful degradation on small terminals.
 * ============================================================================ */

void ui_layout_calculate(Layout *layout, int term_rows, int term_cols) {
    /* Enforce minimum size */
    if (term_rows < MIN_TERM_ROWS) term_rows = MIN_TERM_ROWS;
    if (term_cols < MIN_TERM_COLS) term_cols = MIN_TERM_COLS;
    
    /* Header: top row, full width */
    layout->header_y = 0;
    layout->header_x = 0;
    layout->header_height = 1;
    layout->header_width = term_cols;
    
    /* Sidebar: left column, below header, above footer */
    layout->sidebar_y = 1;
    layout->sidebar_x = 0;
    layout->sidebar_height = term_rows - 3; /* -1 for header, -1 for footer, -1 for spacing */
    layout->sidebar_width = 20; /* Fixed width for sidebar */
    
    /* Content: right of sidebar, below header, above footer */
    layout->content_y = 1;
    layout->content_x = 21; /* Sidebar width + 1 for spacing */
    layout->content_height = term_rows - 3;
    layout->content_width = term_cols - 22; /* Total width - sidebar - spacing */
    
    /* Footer: bottom row, full width */
    layout->footer_y = term_rows - 2;
    layout->footer_x = 0;
    layout->footer_height = 1;
    layout->footer_width = term_cols;
    
    /* Adjust for very small terminals */
    if (term_cols < 100) {
        layout->sidebar_width = 18;
        layout->content_x = 19;
        layout->content_width = term_cols - 20;
    }
    
    if (term_rows < 20) {
        layout->sidebar_height = term_rows - 3;
        layout->content_height = term_rows - 3;
    }
}

void ui_layout_get_header(Layout *layout, int *y, int *x, int *h, int *w) {
    if (y) *y = layout->header_y;
    if (x) *x = layout->header_x;
    if (h) *h = layout->header_height;
    if (w) *w = layout->header_width;
}

void ui_layout_get_sidebar(Layout *layout, int *y, int *x, int *h, int *w) {
    if (y) *y = layout->sidebar_y;
    if (x) *x = layout->sidebar_x;
    if (h) *h = layout->sidebar_height;
    if (w) *w = layout->sidebar_width;
}

void ui_layout_get_content(Layout *layout, int *y, int *x, int *h, int *w) {
    if (y) *y = layout->content_y;
    if (x) *x = layout->content_x;
    if (h) *h = layout->content_height;
    if (w) *w = layout->content_width;
}

void ui_layout_get_footer(Layout *layout, int *y, int *x, int *h, int *w) {
    if (y) *y = layout->footer_y;
    if (x) *x = layout->footer_x;
    if (h) *h = layout->footer_height;
    if (w) *w = layout->footer_width;
}


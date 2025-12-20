#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

/* ============================================================================
 * UI LAYOUT SYSTEM
 * ============================================================================
 * Responsive layout engine that adapts to terminal size.
 * Provides consistent layout regions (header, sidebar, content, footer).
 * ============================================================================ */

/* Layout regions */
typedef struct {
    int header_y;
    int header_x;
    int header_height;
    int header_width;
    
    int sidebar_y;
    int sidebar_x;
    int sidebar_height;
    int sidebar_width;
    
    int content_y;
    int content_x;
    int content_height;
    int content_width;
    
    int footer_y;
    int footer_x;
    int footer_height;
    int footer_width;
} Layout;

/* Calculate layout based on terminal dimensions */
void ui_layout_calculate(Layout *layout, int term_rows, int term_cols);

/* Get layout regions */
void ui_layout_get_header(Layout *layout, int *y, int *x, int *h, int *w);
void ui_layout_get_sidebar(Layout *layout, int *y, int *x, int *h, int *w);
void ui_layout_get_content(Layout *layout, int *y, int *x, int *h, int *w);
void ui_layout_get_footer(Layout *layout, int *y, int *x, int *h, int *w);

/* Minimum terminal size */
#define MIN_TERM_ROWS 24
#define MIN_TERM_COLS 80

#endif /* UI_LAYOUT_H */


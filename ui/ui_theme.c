#include "ui_theme.h"
#include <stdlib.h>

/* ============================================================================
 * UI THEME IMPLEMENTATION
 * ============================================================================
 * Initializes ncurses color pairs with a professional dark theme palette.
 * Colors are carefully chosen for readability and visual appeal.
 * ============================================================================ */

void ui_theme_init(void) {
    /* Initialize color support */
    if (!has_colors()) {
        return; /* Terminal doesn't support colors */
    }
    
    start_color();
    use_default_colors(); /* Use terminal's default background */
    
    /* Dark theme color definitions */
    /* Format: init_pair(pair_id, foreground, background) */
    
    /* Default text - white on dark background */
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, -1);
    
    /* Title - bright cyan for emphasis */
    init_pair(COLOR_PAIR_TITLE, COLOR_CYAN, -1);
    
    /* Header - bright white on dark blue */
    init_pair(COLOR_PAIR_HEADER, COLOR_WHITE, COLOR_BLUE);
    
    /* Sidebar - dim white on dark gray */
    init_pair(COLOR_PAIR_SIDEBAR, COLOR_WHITE, COLOR_BLACK);
    
    /* Sidebar selected - bright white on blue */
    init_pair(COLOR_PAIR_SIDEBAR_SELECTED, COLOR_WHITE, COLOR_BLUE);
    
    /* Content area - default colors */
    init_pair(COLOR_PAIR_CONTENT, COLOR_WHITE, -1);
    
    /* Borders - dim white */
    init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, -1);
    
    /* Success messages - green */
    init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, -1);
    
    /* Warning messages - yellow */
    init_pair(COLOR_PAIR_WARNING, COLOR_YELLOW, -1);
    
    /* Error messages - red */
    init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
    
    /* Info messages - cyan */
    init_pair(COLOR_PAIR_INFO, COLOR_CYAN, -1);
    
    /* Highlight - bright yellow background */
    init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
    
    /* Dim text - gray for secondary information */
    init_pair(COLOR_PAIR_DIM, COLOR_BLACK, -1);
    
    /* Table header - bright white on dark blue */
    init_pair(COLOR_PAIR_TABLE_HEADER, COLOR_WHITE, COLOR_BLUE);
    
    /* Table even rows - subtle background */
    init_pair(COLOR_PAIR_TABLE_EVEN, COLOR_WHITE, COLOR_BLACK);
    
    /* Table odd rows - slightly different for zebra striping */
    init_pair(COLOR_PAIR_TABLE_ODD, COLOR_WHITE, COLOR_BLACK);
    
    /* Table selected row - blue background */
    init_pair(COLOR_PAIR_TABLE_SELECTED, COLOR_WHITE, COLOR_BLUE);
    
    /* Input field - default */
    init_pair(COLOR_PAIR_INPUT, COLOR_WHITE, -1);
    
    /* Input field focused - bright white on dark blue */
    init_pair(COLOR_PAIR_INPUT_FOCUS, COLOR_WHITE, COLOR_BLUE);
}

int ui_theme_get_pair(ColorPair pair) {
    if (pair < COLOR_PAIR_DEFAULT || pair >= COLOR_PAIR_COUNT) {
        return COLOR_PAIR(COLOR_PAIR_DEFAULT);
    }
    return COLOR_PAIR(pair);
}

void ui_theme_draw_box(int y, int x, int height, int width) {
    int i;
    
    /* Draw corners */
    mvaddch(y, x, BOX_ULCORNER);
    mvaddch(y, x + width - 1, BOX_URCORNER);
    mvaddch(y + height - 1, x, BOX_LLCORNER);
    mvaddch(y + height - 1, x + width - 1, BOX_LRCORNER);
    
    /* Draw horizontal lines */
    for (i = x + 1; i < x + width - 1; i++) {
        mvaddch(y, i, BOX_HLINE);
        mvaddch(y + height - 1, i, BOX_HLINE);
    }
    
    /* Draw vertical lines */
    for (i = y + 1; i < y + height - 1; i++) {
        mvaddch(i, x, BOX_VLINE);
        mvaddch(i, x + width - 1, BOX_VLINE);
    }
}

void ui_theme_draw_hline(int y, int x, int width) {
    int i;
    for (i = 0; i < width; i++) {
        mvaddch(y, x + i, BOX_HLINE);
    }
}

void ui_theme_draw_vline(int y, int x, int height) {
    int i;
    for (i = 0; i < height; i++) {
        mvaddch(y + i, x, BOX_VLINE);
    }
}

void ui_theme_draw_title(const char *title, int y, int x, int width) {
    int len = 0;
    const char *p = title;
    while (*p && len < width - 4) {
        len++;
        p++;
    }
    
    int start_x = x + (width - len - 4) / 2;
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(y, start_x, " %s ", title);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));
}


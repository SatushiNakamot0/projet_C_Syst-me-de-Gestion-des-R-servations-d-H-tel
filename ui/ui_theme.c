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

    /* Professional Dark Theme - High Contrast & Readable */
    /* Format: init_pair(pair_id, foreground, background) */

    /* PRIMARY COLORS - Main content, readable text */
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);        /* Primary text: White on black */
    init_pair(COLOR_PAIR_CONTENT, COLOR_WHITE, COLOR_BLACK);        /* Content area: White on black */

    /* SECONDARY COLORS - Metadata, hints, inactive */
    init_pair(COLOR_PAIR_DIM, COLOR_BLACK + 8, COLOR_BLACK);       /* Dim text: Dark gray on black */

    /* ACCENT COLORS - Emphasis, titles, important info */
    init_pair(COLOR_PAIR_TITLE, COLOR_CYAN, COLOR_BLACK);           /* Titles: Bright cyan on black */
    init_pair(COLOR_PAIR_ACCENT, COLOR_CYAN, COLOR_BLACK);          /* Highlights: Bright cyan on black */

    /* STATUS COLORS - Feedback and state indication */
    init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, COLOR_BLACK);        /* Success: Green on black */
    init_pair(COLOR_PAIR_WARNING, COLOR_YELLOW, COLOR_BLACK);       /* Warning: Yellow on black */
    init_pair(COLOR_PAIR_ERROR, COLOR_RED, COLOR_BLACK);            /* Error: Red on black */
    init_pair(COLOR_PAIR_INFO, COLOR_BLUE, COLOR_BLACK);            /* Info: Blue on black */

    /* INTERACTIVE STATES - Focus, selection, disabled */
    init_pair(COLOR_PAIR_HEADER, COLOR_BLACK, COLOR_WHITE);         /* Header: Black on white */
    init_pair(COLOR_PAIR_SIDEBAR, COLOR_WHITE, COLOR_BLACK);        /* Sidebar: White on black */
    init_pair(COLOR_PAIR_SIDEBAR_SELECTED, COLOR_BLACK, COLOR_CYAN); /* Selected: Black on cyan */
    init_pair(COLOR_PAIR_FOCUS, COLOR_BLACK, COLOR_CYAN);           /* Focus: Black on cyan */
    init_pair(COLOR_PAIR_HOVER, COLOR_BLACK, COLOR_BLUE);           /* Hover: Black on blue */
    init_pair(COLOR_PAIR_DISABLED, COLOR_BLACK + 8, COLOR_BLACK);   /* Disabled: Gray on black */

    /* TABLE COLORS - Tabular data display */
    init_pair(COLOR_PAIR_TABLE_HEADER, COLOR_BLACK, COLOR_WHITE);   /* Header: Black on white */
    init_pair(COLOR_PAIR_TABLE_EVEN, COLOR_WHITE, COLOR_BLACK);     /* Even rows: White on black */
    init_pair(COLOR_PAIR_TABLE_ODD, COLOR_WHITE, COLOR_BLACK);      /* Odd rows: White on black */
    init_pair(COLOR_PAIR_TABLE_SELECTED, COLOR_BLACK, COLOR_CYAN);  /* Selected: Black on cyan */

    /* HIGHLIGHT - For highlighting important UI elements */
    init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_BLACK, COLOR_CYAN);       /* Highlight: Black on cyan */

    /* INPUT COLORS - Form fields */
    init_pair(COLOR_PAIR_INPUT, COLOR_WHITE, COLOR_BLACK);          /* Input: White on black */
    init_pair(COLOR_PAIR_INPUT_FOCUS, COLOR_BLACK, COLOR_CYAN);     /* Focused: Black on cyan */

    /* BORDER COLORS - Visual separation */
    init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, COLOR_BLACK);         /* Border: White on black */

    /* SECONDARY BG - Alternative backgrounds */
    init_pair(COLOR_PAIR_SECONDARY_BG, COLOR_BLACK, COLOR_BLACK);   /* Secondary: Black on black */
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


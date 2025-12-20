#ifndef UI_THEME_H
#define UI_THEME_H

#include <ncurses.h>

/* ============================================================================
 * UI THEME SYSTEM
 * ============================================================================
 * Professional color palette and theme definitions for the TUI.
 * Uses ncurses color pairs for consistent theming across the application.
 * ============================================================================ */

/* ============================================================================
 * COLOR SEMANTICS - STRICT RULES
 * ============================================================================
 * Colors are semantic, not arbitrary. Each color has a specific meaning
 * and usage. NEVER use colors outside their semantic purpose.
 * ============================================================================ */

/* Color pair IDs (ncurses uses pairs starting from 1) */
typedef enum {
    /* PRIMARY COLORS - Main content, readable text */
    COLOR_PAIR_DEFAULT = 1,      /* Primary text: White on default bg */
    COLOR_PAIR_CONTENT,          /* Content area: Primary text */
    
    /* SECONDARY COLORS - Metadata, hints, inactive */
    COLOR_PAIR_DIM,              /* Secondary text: Gray, metadata */
    COLOR_PAIR_SECONDARY_BG,     /* Secondary background: Black */
    
    /* ACCENT COLORS - Emphasis, titles, important info */
    COLOR_PAIR_TITLE,            /* Accent: Cyan, titles */
    COLOR_PAIR_ACCENT,           /* Accent: Cyan, highlights */
    
    /* STATUS COLORS - Feedback and state indication */
    COLOR_PAIR_SUCCESS,          /* Success: Green, positive feedback */
    COLOR_PAIR_WARNING,          /* Warning: Yellow, attention needed */
    COLOR_PAIR_ERROR,            /* Error: Red, critical issues */
    COLOR_PAIR_INFO,             /* Info: Blue, informational messages */
    
    /* INTERACTIVE STATES - Focus, selection, disabled */
    COLOR_PAIR_HEADER,           /* Header: White on blue bg */
    COLOR_PAIR_SIDEBAR,          /* Sidebar: White on black bg */
    COLOR_PAIR_SIDEBAR_SELECTED, /* Focus: White on blue bg */
    COLOR_PAIR_FOCUS,            /* Focus: White on blue bg */
    COLOR_PAIR_HOVER,            /* Hover: Slightly brighter blue */
    COLOR_PAIR_DISABLED,         /* Disabled: Gray, unavailable */
    
    /* TABLE COLORS - Tabular data display */
    COLOR_PAIR_TABLE_HEADER,     /* Table header: White on blue bg */
    COLOR_PAIR_TABLE_EVEN,       /* Even rows: White on black bg */
    COLOR_PAIR_TABLE_ODD,        /* Odd rows: White on black bg */
    COLOR_PAIR_TABLE_SELECTED,   /* Selected row: White on blue bg */
    
    /* INPUT COLORS - Form fields */
    COLOR_PAIR_INPUT,            /* Input: White on default bg */
    COLOR_PAIR_INPUT_FOCUS,      /* Input focused: White on blue bg */
    
    /* BORDER COLORS - Visual separation */
    COLOR_PAIR_BORDER,           /* Border: White, visual separation */
    
    COLOR_PAIR_COUNT
} ColorPair;

/* Theme initialization - must be called after initscr() */
void ui_theme_init(void);

/* Get color pair attribute for use with attron/attroff */
int ui_theme_get_pair(ColorPair pair);

/* Box drawing characters */
#define BOX_HLINE     ACS_HLINE      /* ─ */
#define BOX_VLINE     ACS_VLINE      /* │ */
#define BOX_ULCORNER  ACS_ULCORNER   /* ┌ */
#define BOX_URCORNER  ACS_URCORNER   /* ┐ */
#define BOX_LLCORNER  ACS_LLCORNER   /* └ */
#define BOX_LRCORNER  ACS_LRCORNER   /* ┘ */
#define BOX_TTEE      ACS_TTEE       /* ┬ */
#define BOX_BTEE      ACS_BTEE       /* ┴ */
#define BOX_LTEE      ACS_LTEE       /* ├ */
#define BOX_RTEE      ACS_RTEE       /* ┤ */
#define BOX_PLUS      ACS_PLUS       /* ┼ */

/* ASCII Icons (fallback if UTF-8 not available) */
#define ICON_ROOM     "[R]"
#define ICON_CLIENT   "[C]"
#define ICON_RESERV   "[B]"
#define ICON_BILL     "[$]"
#define ICON_SEARCH   "[?]"
#define ICON_ADD      "[+]"
#define ICON_EDIT     "[E]"
#define ICON_DELETE   "[X]"
#define ICON_SAVE     "[S]"
#define ICON_EXIT     "[Q]"
#define ICON_HELP     "[?]"

/* Drawing helpers */
void ui_theme_draw_box(int y, int x, int height, int width);
void ui_theme_draw_hline(int y, int x, int width);
void ui_theme_draw_vline(int y, int x, int height);
void ui_theme_draw_title(const char *title, int y, int x, int width);

#endif /* UI_THEME_H */


#include "ui.h"
#include "ui_state.h"
#include "ui_theme.h"
#include "ui_layout.h"
#include "ui_draw.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif
#include "ui_input.h"
#include "ui_utils.h"
#include "../include/fichiers.h"
#include "../include/clients.h"
#include "../include/facturation.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* ============================================================================
 * MAIN UI IMPLEMENTATION
 * ============================================================================
 * State machine and main UI loop. Coordinates all UI components.
 * ============================================================================ */

static UIContext *g_ctx = NULL;
static bool g_running = false;

/* Signal handler for window resize */
static void handle_resize(int sig) {
    (void)sig; /* Unused parameter */
    if (g_ctx) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        ui_context_resize(g_ctx, rows, cols);
        g_ctx->needs_redraw = true;
    }
}

/* State handler function declarations */
static void state_draw_dashboard(UIContext *ctx);
static NavDirection state_handle_dashboard(UIContext *ctx, int key);
static void state_cleanup_dashboard(UIContext *ctx);

static void state_draw_clients(UIContext *ctx);
static NavDirection state_handle_clients(UIContext *ctx, int key);
static void state_cleanup_clients(UIContext *ctx);

static void state_draw_rooms(UIContext *ctx);
static NavDirection state_handle_rooms(UIContext *ctx, int key);
static void state_cleanup_rooms(UIContext *ctx);

static void state_draw_reservations(UIContext *ctx);
static NavDirection state_handle_reservations(UIContext *ctx, int key);
static void state_cleanup_reservations(UIContext *ctx);

static void state_draw_billing(UIContext *ctx);
static NavDirection state_handle_billing(UIContext *ctx, int key);
static void state_cleanup_billing(UIContext *ctx);

static void state_draw_help(UIContext *ctx);
static NavDirection state_handle_help(UIContext *ctx, int key);
static void state_cleanup_help(UIContext *ctx);

/* State handler table */
static const StateHandler state_handlers[] = {
    [UI_STATE_DASHBOARD] = {
        .draw = state_draw_dashboard,
        .handle_input = state_handle_dashboard,
        .cleanup = state_cleanup_dashboard,
        .name = "Dashboard"
    },
    [UI_STATE_CLIENTS] = {
        .draw = state_draw_clients,
        .handle_input = state_handle_clients,
        .cleanup = state_cleanup_clients,
        .name = "Clients"
    },
    [UI_STATE_ROOMS] = {
        .draw = state_draw_rooms,
        .handle_input = state_handle_rooms,
        .cleanup = state_cleanup_rooms,
        .name = "Rooms"
    },
    [UI_STATE_RESERVATIONS] = {
        .draw = state_draw_reservations,
        .handle_input = state_handle_reservations,
        .cleanup = state_cleanup_reservations,
        .name = "Reservations"
    },
    [UI_STATE_BILLING] = {
        .draw = state_draw_billing,
        .handle_input = state_handle_billing,
        .cleanup = state_cleanup_billing,
        .name = "Billing"
    },
    [UI_STATE_HELP] = {
        .draw = state_draw_help,
        .handle_input = state_handle_help,
        .cleanup = state_cleanup_help,
        .name = "Help"
    }
};

/* UIContext implementation */
UIContext* ui_context_create(void) {
    UIContext *ctx = calloc(1, sizeof(UIContext));
    if (!ctx) return NULL;
    
    ctx->current_state = UI_STATE_DASHBOARD;
    ctx->previous_state = UI_STATE_DASHBOARD;
    ctx->selected_menu_item = 0;
    ctx->selected_list_item = 0;
    ctx->scroll_offset = 0;
    ctx->input_mode = 0;
    ctx->input_cursor_pos = 0;
    ctx->status_timeout = 0;
    ctx->dialog_active = false;
    ctx->needs_redraw = true;
    
    /* Allocate data arrays */
    ctx->clients_capacity = MAX_CLIENTS;
    ctx->clients = calloc(ctx->clients_capacity, sizeof(Client));
    
    ctx->chambres_capacity = MAX_CHAMBRES;
    ctx->chambres = calloc(ctx->chambres_capacity, sizeof(Chambre));
    
    ctx->reservations_capacity = MAX_RESERVATIONS;
    ctx->reservations = calloc(ctx->reservations_capacity, sizeof(Reservation));
    
    ctx->factures_capacity = MAX_FACTURES;
    ctx->factures = calloc(ctx->factures_capacity, sizeof(Facture));
    
    if (!ctx->clients || !ctx->chambres || !ctx->reservations || !ctx->factures) {
        ui_context_destroy(ctx);
        return NULL;
    }
    
    return ctx;
}

void ui_context_destroy(UIContext *ctx) {
    if (!ctx) return;
    
    free(ctx->clients);
    free(ctx->chambres);
    free(ctx->reservations);
    free(ctx->factures);
    free(ctx->search_results);
    free(ctx);
}

void ui_context_resize(UIContext *ctx, int rows, int cols) {
    if (!ctx) return;
    ctx->term_rows = rows;
    ctx->term_cols = cols;
    ctx->max_visible_items = rows - 8; /* Account for headers/footers */
}

bool ui_context_load_data(UIContext *ctx) {
    if (!ctx) return false;
    
    /* Load all data using fichiers module */
    charger_clients(ctx->clients, &ctx->clients_count);
    charger_chambres(ctx->chambres, &ctx->chambres_count);
    charger_reservations(ctx->reservations, &ctx->reservations_count);
    charger_factures(ctx->factures, &ctx->factures_count);
    
    return true;
}

void ui_context_save_data(UIContext *ctx) {
    if (!ctx) return;
    
    sauvegarder_clients(ctx->clients, ctx->clients_count);
    sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
    sauvegarder_reservations(ctx->reservations, ctx->reservations_count);
    sauvegarder_factures(ctx->factures, ctx->factures_count);
}

/* State implementations */
static void state_draw_dashboard(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_dashboard(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_dashboard(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    
    if (dir == NAV_UP && ctx->selected_menu_item > 0) {
        ctx->selected_menu_item--;
        ctx->needs_redraw = true;
    } else if (dir == NAV_DOWN && ctx->selected_menu_item < 6) {
        ctx->selected_menu_item++;
        ctx->needs_redraw = true;
    } else if (dir == NAV_SELECT) {
        switch (ctx->selected_menu_item) {
            case 0: ctx->current_state = UI_STATE_DASHBOARD; break;
            case 1: ctx->current_state = UI_STATE_CLIENTS; break;
            case 2: ctx->current_state = UI_STATE_ROOMS; break;
            case 3: ctx->current_state = UI_STATE_RESERVATIONS; break;
            case 4: ctx->current_state = UI_STATE_BILLING; break;
            case 5: ctx->current_state = UI_STATE_HELP; break;
            case 6: ctx->current_state = UI_STATE_EXIT; break;
        }
        ctx->needs_redraw = true;
    }
    
    return dir;
}

static void state_cleanup_dashboard(UIContext *ctx) {
    (void)ctx; /* Unused */
}

static void state_draw_clients(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_clients_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    
    /* Handle sidebar navigation */
    if (dir == NAV_UP || dir == NAV_DOWN) {
        if (dir == NAV_UP && ctx->selected_menu_item > 0) {
            ctx->selected_menu_item--;
        } else if (dir == NAV_DOWN && ctx->selected_menu_item < 6) {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }
    
    /* Handle list navigation */
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    } else if (key == KEY_DOWN && ctx->selected_list_item < ctx->clients_count - 1) {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }
    
    /* Handle actions */
    if (key == 'a' || key == 'A') {
        ctx->current_state = UI_STATE_CLIENTS_ADD;
        ctx->needs_redraw = true;
    } else if (key == 'e' || key == 'E') {
        if (ctx->selected_list_item < ctx->clients_count) {
            ctx->current_state = UI_STATE_CLIENTS_EDIT;
            ctx->needs_redraw = true;
        }
    } else if (key == 'd' || key == 'D') {
        if (ctx->selected_list_item < ctx->clients_count) {
            ctx->current_state = UI_STATE_CLIENTS_DELETE;
            ctx->needs_redraw = true;
        }
    } else if (key == 's' || key == 'S') {
        ctx->current_state = UI_STATE_CLIENTS_SEARCH;
        ctx->needs_redraw = true;
    } else if (dir == NAV_BACK) {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }
    
    return dir;
}

static void state_cleanup_clients(UIContext *ctx) {
    (void)ctx;
}

static void state_draw_rooms(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_rooms_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_rooms(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    
    /* Handle sidebar navigation */
    if (dir == NAV_UP || dir == NAV_DOWN) {
        if (dir == NAV_UP && ctx->selected_menu_item > 0) {
            ctx->selected_menu_item--;
        } else if (dir == NAV_DOWN && ctx->selected_menu_item < 6) {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }
    
    /* Handle list navigation */
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    } else if (key == KEY_DOWN && ctx->selected_list_item < ctx->chambres_count - 1) {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }
    
    /* Handle actions */
    if (key == 'a' || key == 'A') {
        ctx->current_state = UI_STATE_ROOMS_ADD;
        ctx->needs_redraw = true;
    } else if (key == 'e' || key == 'E') {
        if (ctx->selected_list_item < ctx->chambres_count) {
            ctx->current_state = UI_STATE_ROOMS_EDIT;
            ctx->needs_redraw = true;
        }
    } else if (key == 'd' || key == 'D') {
        if (ctx->selected_list_item < ctx->chambres_count) {
            /* Delete room - would trigger confirmation dialog */
            ctx->needs_redraw = true;
        }
    } else if (key == 's' || key == 'S') {
        /* Search rooms */
        ctx->needs_redraw = true;
    } else if (dir == NAV_BACK) {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }
    
    return dir;
}

static void state_cleanup_rooms(UIContext *ctx) {
    (void)ctx;
}

static void state_draw_reservations(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_reservations_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_reservations(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    
    /* Handle sidebar navigation */
    if (dir == NAV_UP || dir == NAV_DOWN) {
        if (dir == NAV_UP && ctx->selected_menu_item > 0) {
            ctx->selected_menu_item--;
        } else if (dir == NAV_DOWN && ctx->selected_menu_item < 6) {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }
    
    /* Handle list navigation */
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    } else if (key == KEY_DOWN && ctx->selected_list_item < ctx->reservations_count - 1) {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }
    
    /* Handle actions */
    if (key == 'a' || key == 'A') {
        ctx->current_state = UI_STATE_RESERVATIONS_ADD;
        ctx->needs_redraw = true;
    } else if (key == 'e' || key == 'E') {
        if (ctx->selected_list_item < ctx->reservations_count) {
            ctx->current_state = UI_STATE_RESERVATIONS_EDIT;
            ctx->needs_redraw = true;
        }
    } else if (key == 'c' || key == 'C') {
        if (ctx->selected_list_item < ctx->reservations_count) {
            /* Cancel reservation - would trigger confirmation dialog */
            ctx->needs_redraw = true;
        }
    } else if (dir == NAV_BACK) {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }
    
    return dir;
}

static void state_cleanup_reservations(UIContext *ctx) {
    (void)ctx;
}

static void state_draw_billing(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_billing_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_billing(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    
    if (key == 'c' || key == 'C') {
        ctx->current_state = UI_STATE_BILLING_CREATE;
        ctx->needs_redraw = true;
    } else if (dir == NAV_BACK) {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }
    
    return dir;
}

static void state_cleanup_billing(UIContext *ctx) {
    (void)ctx;
}

static void state_draw_help(UIContext *ctx) {
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_help(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_help(UIContext *ctx, int key) {
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == KEY_F(1)) {
        ctx->current_state = ctx->previous_state;
        ctx->needs_redraw = true;
    }
    return dir;
}

static void state_cleanup_help(UIContext *ctx) {
    (void)ctx;
}

/* Main UI functions */
bool ui_init(UIContext *ctx) {
    if (!ctx) return false;
    
    g_ctx = ctx;
    
    /* Initialize ncurses */
    initscr();
    if (!stdscr) {
        return false;
    }
    
    /* Get terminal size */
    getmaxyx(stdscr, ctx->term_rows, ctx->term_cols);
    ui_context_resize(ctx, ctx->term_rows, ctx->term_cols);
    
    /* Setup ncurses */
    cbreak();
    noecho();
    curs_set(0); /* Hide cursor */
    ui_input_init();
    ui_theme_init();
    
    /* Setup resize handler */
#ifndef _WIN32
    signal(SIGWINCH, handle_resize);
#else
    /* On Windows, we'll handle resizing in the main loop */
    SetConsoleCtrlHandler(NULL, FALSE);
#endif
    
    /* Load data */
    ui_context_load_data(ctx);
    
    return true;
}

#ifdef _WIN32
static BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        g_running = false;
        return TRUE;
    }
    return FALSE;
}
#endif

void ui_run(UIContext *ctx) {
    if (!ctx) return;
    
    g_running = true;
    Layout layout;
    
#ifdef _WIN32
    /* Set up console handler for Windows */
    SetConsoleCtrlHandler(consoleHandler, TRUE);
    
    /* Get initial console size */
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    ctx->term_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ctx->term_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    ui_context_resize(ctx, ctx->term_rows, ctx->term_cols);
#endif
    
    while (g_running && ctx->current_state != UI_STATE_EXIT) {
#ifdef _WIN32
        /* Check for window resize on Windows */
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int new_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int new_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        
        if (new_cols != ctx->term_cols || new_rows != ctx->term_rows) {
            ctx->term_cols = new_cols;
            ctx->term_rows = new_rows;
            ui_context_resize(ctx, new_rows, new_cols);
            ctx->needs_redraw = true;
        }
#endif
        
        /* Calculate layout */
        ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
        
        /* Draw current state */
        if (ctx->needs_redraw || ctx->current_state != ctx->previous_state) {
            clear();
            
            UIState state = ctx->current_state;
            if (state < UI_STATE_COUNT && state_handlers[state].draw) {
                state_handlers[state].draw(ctx);
            }
            
            refresh();
            ctx->needs_redraw = false;
            ctx->previous_state = ctx->current_state;
        }
        
        /* Process input */
        int key = ui_input_get_key();
        if (key != ERR) {
            UIState state = ctx->current_state;
            if (state < UI_STATE_COUNT && state_handlers[state].handle_input) {
                NavDirection dir = state_handlers[state].handle_input(ctx, key);
                if (dir == NAV_SELECT && ctx->current_state == UI_STATE_EXIT) {
                    g_running = false;
                }
            }
        }
        
        /* Small delay to prevent CPU spinning */
        napms(50); /* 50ms = ~20 FPS */
    }
    
    /* Save data before exit */
    ui_context_save_data(ctx);
}

void ui_cleanup(UIContext *ctx) {
    if (!ctx) return;
    
    /* Cleanup current state */
    UIState state = ctx->current_state;
    if (state < UI_STATE_COUNT && state_handlers[state].cleanup) {
        state_handlers[state].cleanup(ctx);
    }
    
    /* End ncurses */
    endwin();
    
    g_ctx = NULL;
    g_running = false;
}

void ui_redraw(UIContext *ctx) {
    if (ctx) {
        ctx->needs_redraw = true;
    }
}


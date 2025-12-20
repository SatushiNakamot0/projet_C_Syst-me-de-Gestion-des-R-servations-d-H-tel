#ifndef UI_STATE_H
#define UI_STATE_H

#include <stdbool.h>
#include "../include/structures.h"

/* ============================================================================
 * UI STATE MACHINE DEFINITIONS
 * ============================================================================
 * This file defines all possible UI states and the context structure
 * that maintains the application state across state transitions.
 * ============================================================================ */

/* UI State enumeration - represents all possible screens */
typedef enum {
    UI_STATE_DASHBOARD = 0,
    UI_STATE_CLIENTS,
    UI_STATE_CLIENTS_LIST,
    UI_STATE_CLIENTS_ADD,
    UI_STATE_CLIENTS_EDIT,
    UI_STATE_CLIENTS_DELETE,
    UI_STATE_CLIENTS_SEARCH,
    UI_STATE_ROOMS,
    UI_STATE_ROOMS_LIST,
    UI_STATE_ROOMS_ADD,
    UI_STATE_ROOMS_EDIT,
    UI_STATE_RESERVATIONS,
    UI_STATE_RESERVATIONS_LIST,
    UI_STATE_RESERVATIONS_ADD,
    UI_STATE_RESERVATIONS_EDIT,
    UI_STATE_BILLING,
    UI_STATE_BILLING_LIST,
    UI_STATE_BILLING_CREATE,
    UI_STATE_HELP,
    UI_STATE_EXIT,
    UI_STATE_COUNT  /* Sentinel value for array sizing */
} UIState;

/* Navigation direction for menu navigation */
typedef enum {
    NAV_UP = 0,
    NAV_DOWN,
    NAV_LEFT,
    NAV_RIGHT,
    NAV_SELECT,
    NAV_BACK,
    NAV_NONE
} NavDirection;

/* UI Context - maintains all application state */
typedef struct {
    /* Current UI state */
    UIState current_state;
    UIState previous_state;
    
    /* Application data */
    Client *clients;
    int clients_count;
    int clients_capacity;
    
    Chambre *chambres;
    int chambres_count;
    int chambres_capacity;
    
    Reservation *reservations;
    int reservations_count;
    int reservations_capacity;
    
    Facture *factures;
    int factures_count;
    int factures_capacity;
    
    /* UI Navigation state */
    int selected_menu_item;      /* Selected item in sidebar */
    int selected_list_item;       /* Selected item in content area */
    int scroll_offset;            /* Scroll position in lists */
    int max_visible_items;        /* Max items visible in current view */
    
    /* Form input state */
    char input_buffer[256];       /* Current input buffer */
    int input_cursor_pos;         /* Cursor position in input */
    int input_mode;               /* 0=normal, 1=insert, 2=search */
    
    /* Search state */
    char search_query[128];
    int search_results_count;
    int *search_results;          /* Array of indices matching search */
    
    /* Notification/Message state */
    char status_message[256];
    int status_type;              /* 0=info, 1=success, 2=warning, 3=error */
    int status_timeout;           /* Frames until message clears */
    
    /* Terminal dimensions */
    int term_rows;
    int term_cols;
    bool needs_redraw;            /* Flag to force full redraw */
    
    /* Dialog/Modal state */
    bool dialog_active;
    char dialog_title[64];
    char dialog_message[256];
    int dialog_result;            /* User's choice in dialog */
    
} UIContext;

/* Function pointer type for state handlers */
typedef void (*StateDrawFunc)(UIContext *ctx);
typedef NavDirection (*StateInputFunc)(UIContext *ctx, int key);
typedef void (*StateCleanupFunc)(UIContext *ctx);

/* State handler structure */
typedef struct {
    StateDrawFunc draw;
    StateInputFunc handle_input;
    StateCleanupFunc cleanup;
    const char *name;  /* For debugging */
} StateHandler;

/* Forward declarations */
UIContext* ui_context_create(void);
void ui_context_destroy(UIContext *ctx);
void ui_context_resize(UIContext *ctx, int rows, int cols);
bool ui_context_load_data(UIContext *ctx);
void ui_context_save_data(UIContext *ctx);

#endif /* UI_STATE_H */


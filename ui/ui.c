#include "ui.h"
#include "ui_state.h"
#include "ui_theme.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "ui_form.h"
#include "ui_reservations.h"
#include "ui_clients.h"
#include "ui_rooms.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif
#include "ui_input.h"
#include "ui_utils.h"
#include "../include/debug.h"
#include "../include/fichiers.h"
#include "../include/clients.h"
#include "../include/chambres.h"
#include "../include/facturation.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* ============================================================================
 * MAIN UI IMPLEMENTATION
 * ============================================================================
 * State machine w l-loop l-kbir dyal l-UI. Kaynssaq kolchi dyal UI components.
 * ============================================================================ */

static UIContext *g_ctx = NULL;
static bool g_running = false;
static UIState g_help_return_state = UI_STATE_DASHBOARD;

/* Handler dyal signal باش ila تبدلات taille dyal l-window */
#ifndef _WIN32
static void handle_resize(int sig)
{
    (void)sig; /* parameter ma msta3mlinch */
    if (g_ctx)
    {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        ui_context_resize(g_ctx, rows, cols);
        g_ctx->needs_redraw = true;
    }
}
#endif

/* Declarations dyal state handlers */
static void state_draw_dashboard(UIContext *ctx);
static NavDirection state_handle_dashboard(UIContext *ctx, int key);
static void state_cleanup_dashboard(UIContext *ctx);

/* State machine input handler */
static NavDirection handle_app_state_input(UIContext *ctx, int key);
static void draw_content_window(UIContext *ctx, Layout *layout);
static bool is_form_state(UIState state);

static void state_draw_clients(UIContext *ctx);
static NavDirection state_handle_clients(UIContext *ctx, int key);
static void state_cleanup_clients(UIContext *ctx);

static void state_draw_clients_add(UIContext *ctx);
static NavDirection state_handle_clients_add(UIContext *ctx, int key);
static void state_cleanup_clients_add(UIContext *ctx);

static void state_draw_clients_edit(UIContext *ctx);
static NavDirection state_handle_clients_edit(UIContext *ctx, int key);
static void state_cleanup_clients_edit(UIContext *ctx);

static void state_draw_clients_search(UIContext *ctx);
static NavDirection state_handle_clients_search(UIContext *ctx, int key);
static void state_cleanup_clients_search(UIContext *ctx);

static void state_draw_rooms(UIContext *ctx);
static NavDirection state_handle_rooms(UIContext *ctx, int key);
static void state_cleanup_rooms(UIContext *ctx);

static void state_draw_rooms_add(UIContext *ctx);
static NavDirection state_handle_rooms_add(UIContext *ctx, int key);
static void state_cleanup_rooms_add(UIContext *ctx);

static void state_draw_reservations(UIContext *ctx);
static NavDirection state_handle_reservations(UIContext *ctx, int key);
static void state_cleanup_reservations(UIContext *ctx);

static void state_draw_reservations_add(UIContext *ctx);
static NavDirection state_handle_reservations_add(UIContext *ctx, int key);
static void state_cleanup_reservations_add(UIContext *ctx);

static void state_draw_billing(UIContext *ctx);
static NavDirection state_handle_billing(UIContext *ctx, int key);
static void state_cleanup_billing(UIContext *ctx);

static void state_draw_billing_create(UIContext *ctx);
static NavDirection state_handle_billing_create(UIContext *ctx, int key);
static void state_cleanup_billing_create(UIContext *ctx);

static void state_draw_help(UIContext *ctx);
static NavDirection state_handle_help(UIContext *ctx, int key);
static void state_cleanup_help(UIContext *ctx);

/* Tableau dyal state handlers */
static const StateHandler state_handlers[] = {
    [UI_STATE_DASHBOARD] = {
        .draw = state_draw_dashboard,
        .handle_input = state_handle_dashboard,
        .cleanup = state_cleanup_dashboard,
        .name = "Dashboard"},
    [UI_STATE_CLIENTS] = {.draw = state_draw_clients, .handle_input = state_handle_clients, .cleanup = state_cleanup_clients, .name = "Clients"},
    [UI_STATE_CLIENTS_ADD] = {.draw = state_draw_clients_add, .handle_input = state_handle_clients_add, .cleanup = state_cleanup_clients_add, .name = "ClientsAdd"},
    [UI_STATE_CLIENTS_EDIT] = {.draw = state_draw_clients_edit, .handle_input = state_handle_clients_edit, .cleanup = state_cleanup_clients_edit, .name = "ClientsEdit"},
    [UI_STATE_CLIENTS_SEARCH] = {.draw = state_draw_clients_search, .handle_input = state_handle_clients_search, .cleanup = state_cleanup_clients_search, .name = "ClientsSearch"},
    [UI_STATE_ROOMS] = {.draw = state_draw_rooms, .handle_input = state_handle_rooms, .cleanup = state_cleanup_rooms, .name = "Rooms"},
    [UI_STATE_ROOMS_ADD] = {.draw = state_draw_rooms_add, .handle_input = state_handle_rooms_add, .cleanup = state_cleanup_rooms_add, .name = "RoomsAdd"},
    [UI_STATE_RESERVATIONS] = {.draw = state_draw_reservations, .handle_input = state_handle_reservations, .cleanup = state_cleanup_reservations, .name = "Reservations"},
    [UI_STATE_RESERVATIONS_ADD] = {.draw = state_draw_reservations_add, .handle_input = state_handle_reservations_add, .cleanup = state_cleanup_reservations_add, .name = "ReservationsAdd"},
    [UI_STATE_BILLING] = {.draw = state_draw_billing, .handle_input = state_handle_billing, .cleanup = state_cleanup_billing, .name = "Billing"},
    [UI_STATE_BILLING_CREATE] = {.draw = state_draw_billing_create, .handle_input = state_handle_billing_create, .cleanup = state_cleanup_billing_create, .name = "BillingCreate"},
    [UI_STATE_HELP] = {.draw = state_draw_help, .handle_input = state_handle_help, .cleanup = state_cleanup_help, .name = "Help"}};

/* Implementation dyal UIContext */
UIContext *ui_context_create(void)
{
    UIContext *ctx = calloc(1, sizeof(UIContext));
    if (!ctx)
    {
        LOG_ERROR("Out of memory: failed to allocate UIContext");
        return NULL;
    }

    ctx->current_state = UI_STATE_DASHBOARD;
    ctx->previous_state = UI_STATE_DASHBOARD;
    ctx->app_state = STATE_SIDEBAR; /* Start with sidebar focus */
    ctx->selected_menu_item = 0;
    ctx->selected_list_item = 0;
    ctx->scroll_offset = 0;
    ctx->input_mode = 0;
    ctx->input_cursor_pos = 0;
    ctx->status_timeout = 0;
    ctx->dialog_active = false;
    ctx->needs_redraw = true;

    /* Initialize windows to NULL */
    ctx->header_win = NULL;
    ctx->sidebar_win = NULL;
    ctx->content_win = NULL;
    ctx->footer_win = NULL;
    ctx->status_win = NULL;

    /* Kan-allociw arrays dyal data */
    ctx->clients_capacity = MAX_CLIENTS;
    ctx->clients = calloc(ctx->clients_capacity, sizeof(Client));
    if (!ctx->clients)
    {
        LOG_ERROR("Out of memory: failed to allocate clients array (capacity=%d)", ctx->clients_capacity);
        ui_context_destroy(ctx);
        return NULL;
    }

    ctx->chambres_capacity = MAX_CHAMBRES;
    ctx->chambres = calloc(ctx->chambres_capacity, sizeof(Chambre));
    if (!ctx->chambres)
    {
        LOG_ERROR("Out of memory: failed to allocate chambres array (capacity=%d)", ctx->chambres_capacity);
        ui_context_destroy(ctx);
        return NULL;
    }

    ctx->reservations_capacity = MAX_RESERVATIONS;
    ctx->reservations = calloc(ctx->reservations_capacity, sizeof(Reservation));
    if (!ctx->reservations)
    {
        LOG_ERROR("Out of memory: failed to allocate reservations array (capacity=%d)", ctx->reservations_capacity);
        ui_context_destroy(ctx);
        return NULL;
    }

    ctx->factures_capacity = MAX_FACTURES;
    ctx->factures = calloc(ctx->factures_capacity, sizeof(Facture));
    if (!ctx->factures)
    {
        LOG_ERROR("Out of memory: failed to allocate factures array (capacity=%d)", ctx->factures_capacity);
        ui_context_destroy(ctx);
        return NULL;
    }

    return ctx;
}

void ui_context_destroy(UIContext *ctx)
{
    if (!ctx)
        return;

    // Clean up form resources
    ui_form_cleanup(ctx);

    free(ctx->clients);
    free(ctx->chambres);
    free(ctx->reservations);
    free(ctx->factures);
    free(ctx->search_results);
    free(ctx);
}

void ui_context_resize(UIContext *ctx, int rows, int cols)
{
    if (!ctx)
        return;
    ctx->term_rows = rows;
    ctx->term_cols = cols;
    ctx->max_visible_items = rows - 8; /* Kan7sbo l-headers/footers */
}

bool ui_context_load_data(UIContext *ctx)
{
    if (!ctx)
        return false;

    /* Kan-chargiw ga3 data b module dyal fichiers */
    charger_clients(ctx->clients, &ctx->clients_count);
    charger_chambres(ctx->chambres, &ctx->chambres_count);
    charger_reservations(ctx->reservations, &ctx->reservations_count);
    charger_factures(ctx->factures, &ctx->factures_count);

    return true;
}

void ui_context_save_data(UIContext *ctx)
{
    if (!ctx)
        return;

    sauvegarder_clients(ctx->clients, ctx->clients_count);
    sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
    sauvegarder_reservations(ctx->reservations, ctx->reservations_count);
    sauvegarder_factures(ctx->factures, ctx->factures_count);
}

/* State machine input handler */
static NavDirection handle_app_state_input(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    switch (ctx->app_state)
    {
    case STATE_SIDEBAR:
        /* UP/DOWN: Changes the active menu selection */
        if (dir == NAV_UP && ctx->selected_menu_item > 0)
        {
            ctx->selected_menu_item--;
            ctx->needs_redraw = true;
        }
        else if (dir == NAV_DOWN && ctx->selected_menu_item < 6)
        {
            ctx->selected_menu_item++;
            ctx->needs_redraw = true;
        }
        /* ENTER: Locks the choice and changes state to STATE_CONTENT_LIST */
        else if (dir == NAV_SELECT)
        {
            switch (ctx->selected_menu_item)
            {
            case 0:
                ctx->current_state = UI_STATE_DASHBOARD;
                break;
            case 1:
                ctx->current_state = UI_STATE_CLIENTS;
                break;
            case 2:
                ctx->current_state = UI_STATE_ROOMS;
                break;
            case 3:
                ctx->current_state = UI_STATE_RESERVATIONS;
                break;
            case 4:
                ctx->current_state = UI_STATE_BILLING;
                break;
            case 5:
                ctx->current_state = UI_STATE_HELP;
                break;
            case 6:
                ctx->current_state = UI_STATE_EXIT;
                break;
            }
            ctx->app_state = STATE_CONTENT_LIST; /* Switch focus to content */
            ctx->selected_list_item = 0;         /* Reset list selection */
            ctx->scroll_offset = 0;              /* Reset scroll */
            ctx->needs_redraw = true;
        }
        break;

    case STATE_CONTENT_LIST:
        /* UP/DOWN: Scrolls through the list */
        if (key == KEY_UP)
        {
            if (ctx->selected_list_item > 0)
            {
                ctx->selected_list_item--;
                if (ctx->selected_list_item < ctx->scroll_offset)
                {
                    ctx->scroll_offset = ctx->selected_list_item;
                }
                ctx->needs_redraw = true;
            }
        }
        else if (key == KEY_DOWN)
        {
            int max_items = 0;
            switch (ctx->current_state)
            {
            case UI_STATE_CLIENTS:
                max_items = ctx->clients_count;
                break;
            case UI_STATE_ROOMS:
                max_items = ctx->chambres_count;
                break;
            case UI_STATE_RESERVATIONS:
                max_items = ctx->reservations_count;
                break;
            case UI_STATE_BILLING:
                max_items = ctx->factures_count;
                break;
            default:
                max_items = 0;
                break;
            }

            if (ctx->selected_list_item < max_items - 1)
            {
                ctx->selected_list_item++;
                if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items)
                {
                    ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
                }
                ctx->needs_redraw = true;
            }
        }
        /* ESC: Unlocks focus and changes state back to STATE_SIDEBAR */
        else if (key == 27) /* ESC */
        {
            clear();
            ctx->app_state = STATE_SIDEBAR;
            ctx->needs_redraw = true;
            /* Force immediate redraw */
            if (ctx->current_state < UI_STATE_COUNT && state_handlers[ctx->current_state].draw)
            {
                state_handlers[ctx->current_state].draw(ctx);
            }
            refresh();
        }
        /* ENTER: Selects the highlighted item */
        else if (dir == NAV_SELECT)
        {
            /* For now, just handle add actions */
            if (key == 'a' || key == 'A')
            {
                if (ctx->current_state == UI_STATE_CLIENTS)
                {
                    ctx->current_state = UI_STATE_CLIENTS_ADD;
                    ctx->app_state = STATE_FORM_INPUT;
                }
                else if (ctx->current_state == UI_STATE_ROOMS)
                {
                    ctx->current_state = UI_STATE_ROOMS_ADD;
                    ctx->app_state = STATE_FORM_INPUT;
                }
                ctx->needs_redraw = true;
            }
            /* Handle other actions like edit, delete */
        }
        break;

    case STATE_FORM_INPUT:
        /* Form input is handled by individual state handlers */
        /* ESC should go back to content list */
        if (key == 27)
        {
            // Clean up form state and return to content list
            ui_form_cleanup(ctx);
            clear();
            ctx->app_state = STATE_CONTENT_LIST;
            ctx->needs_redraw = true;
            /* Force immediate redraw */
            if (ctx->current_state < UI_STATE_COUNT && state_handlers[ctx->current_state].draw)
            {
                state_handlers[ctx->current_state].draw(ctx);
            }
            refresh();
            return NAV_BACK;
        }
        break;
    }

    return dir;
}

/* Draw content window based on current menu selection */
static void draw_content_window(UIContext *ctx, Layout *layout)
{
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    /* Clear content area */
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    /* Highlight content border if in content focus */
    if (ctx->app_state == STATE_CONTENT_LIST)
    {
        attron(A_REVERSE);
        ui_theme_draw_box(y, x, h, w);
        attroff(A_REVERSE);
    }
    else
    {
        ui_theme_draw_box(y, x, h, w);
    }

    /* Draw content based on current state */
    switch (ctx->current_state)
    {
    case UI_STATE_DASHBOARD:
        ui_draw_dashboard(ctx, layout);
        break;
    case UI_STATE_CLIENTS:
        ui_draw_clients_list(ctx, layout);
        break;
    case UI_STATE_ROOMS:
        ui_draw_rooms_list(ctx, layout);
        break;
    case UI_STATE_RESERVATIONS:
        ui_draw_reservations_list(ctx, layout);
        break;
    case UI_STATE_BILLING:
        ui_draw_billing_list(ctx, layout);
        break;
    case UI_STATE_HELP:
        ui_draw_help(ctx, layout);
        break;
    default:
        /* For form states, they handle their own drawing */
        break;
    }
}

/* State implementations */
static void state_draw_dashboard(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_dashboard(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_dashboard(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    if (dir == NAV_UP && ctx->selected_menu_item > 0)
    {
        ctx->selected_menu_item--;
        ctx->needs_redraw = true;
    }
    else if (dir == NAV_DOWN && ctx->selected_menu_item < 6)
    {
        ctx->selected_menu_item++;
        ctx->needs_redraw = true;
    }
    else if (dir == NAV_SELECT)
    {
        switch (ctx->selected_menu_item)
        {
        case 0:
            ctx->current_state = UI_STATE_DASHBOARD;
            break;
        case 1:
            ctx->current_state = UI_STATE_CLIENTS;
            break;
        case 2:
            ctx->current_state = UI_STATE_ROOMS;
            break;
        case 3:
            ctx->current_state = UI_STATE_RESERVATIONS;
            break;
        case 4:
            ctx->current_state = UI_STATE_BILLING;
            break;
        case 5:
            ctx->current_state = UI_STATE_HELP;
            break;
        case 6:
            ctx->current_state = UI_STATE_EXIT;
            break;
        }
        ctx->needs_redraw = true;
    }

    return dir;
}

static void state_cleanup_dashboard(UIContext *ctx)
{
    (void)ctx; /* Unused */
}

static void state_draw_clients(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    draw_content_window(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients(UIContext *ctx, int key)
{
    return (NavDirection)handle_clients_input(ctx, key);
}




static void state_cleanup_clients(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_clients_add(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_clients_add(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients_add(UIContext *ctx, int key)
{
    if (!ctx->in_input_mode)
    {
        // Initialize form if first time
        ctx->num_fields = 4;
        ctx->current_field = 0;
        ui_form_start_input(ctx, 1); // Start in text input mode
        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    // Handle form input
    int form_result = ui_form_handle_input(ctx, key);

    if (form_result == FORM_CANCEL)
    {
        ui_form_cleanup(ctx);
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
        return NAV_BACK;
    }
    else if (form_result == FORM_SUBMIT)
    {
        // Submit form
        if (ctx->field_values[0] && ctx->field_values[1] &&
            ctx->field_values[2] && ctx->field_values[3])
        {

            // Add new client to the array
            Client new_client;
            strncpy(new_client.nom, ctx->field_values[0], sizeof(new_client.nom) - 1);
            new_client.nom[sizeof(new_client.nom) - 1] = '\0';
            strncpy(new_client.prenom, ctx->field_values[1], sizeof(new_client.prenom) - 1);
            new_client.prenom[sizeof(new_client.prenom) - 1] = '\0';
            strncpy(new_client.email, ctx->field_values[2], sizeof(new_client.email) - 1);
            new_client.email[sizeof(new_client.email) - 1] = '\0';
            strncpy(new_client.telephone, ctx->field_values[3], sizeof(new_client.telephone) - 1);
            new_client.telephone[sizeof(new_client.telephone) - 1] = '\0';
            // ID is handled by client_ajouter

            int res = client_ajouter(ctx->clients, &ctx->clients_count, &new_client);
            if (res == 0)
            {
                LOG_INFO("Client saved successfully");
                strcpy(ctx->status_message, "Client added successfully");
                ctx->status_type = 1; // Success
                ctx->status_timeout = 60;
                ui_form_cleanup(ctx);
                ctx->current_state = UI_STATE_CLIENTS;
                ctx->app_state = STATE_CONTENT_LIST;
                ctx->needs_redraw = true;
                return NAV_BACK;
            }
            else if (res == -2)
            {
                LOG_ERROR("Cannot add client: duplicate email");
                strcpy(ctx->status_message, "Error: Email already exists");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
            }
            else
            {
                LOG_ERROR("Cannot add client: database error %d", res);
                strcpy(ctx->status_message, "Error adding client");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
            }
        }
        else
        {
            // Error - missing required fields
            LOG_ERROR("Cannot add client: missing required fields");
            strcpy(ctx->status_message, "All fields are required");
            ctx->status_type = 3; // Error
            ctx->status_timeout = 60;
        }
    }

    ctx->needs_redraw = true;
    return NAV_NONE;
}

static void state_cleanup_clients_add(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_clients_edit(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_clients_edit(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients_edit(UIContext *ctx, int key)
{
    (void)key; /* Unused parameter */

    // Use the new refactored form function
    if (ctx->selected_list_item < ctx->clients_count)
    {
        Client *selected_client = &ctx->clients[ctx->selected_list_item];

        int result = show_edit_client_form(selected_client, ctx->clients, ctx->clients_count);

        if (result == 0)
        {
            // Success
            strcpy(ctx->status_message, "Client updated successfully");
            ctx->status_type = 1; // Success
            ctx->status_timeout = 60;
        }
        else
        {
            // Cancelled
            strcpy(ctx->status_message, "Edit cancelled");
            ctx->status_type = 0; // Info
            ctx->status_timeout = 30;
        }

        // Return to clients list
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
        return NAV_BACK;
    }

    // Fallback if no client selected
    ctx->current_state = UI_STATE_CLIENTS;
    ctx->app_state = STATE_CONTENT_LIST;
    ctx->needs_redraw = true;
    return NAV_BACK;
}

static void state_cleanup_clients_edit(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_clients_search(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_clients_search(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients_search(UIContext *ctx, int key)
{
    // Initialize search form if first time
    if (!ctx->in_input_mode)
    {
        ctx->num_fields = 1;
        ctx->current_field = 0;
        ui_form_start_input(ctx, 1); // Start in text input mode

        if (!ctx->field_values)
        {
            ctx->field_values = calloc(ctx->num_fields, sizeof(char *));
            ctx->field_values[0] = strdup("");
        }

        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    // Handle form input
    int form_result = ui_form_handle_input(ctx, key);

    if (form_result == FORM_CANCEL || key == 27)
    {
        ui_form_cleanup(ctx);
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
        return NAV_BACK;
    }
    else if (form_result == FORM_SUBMIT)
    {
        // Perform search
        if (ctx->field_values[0] && strlen(ctx->field_values[0]) > 0)
        {
            // Simple search implementation - filter clients by name
            free(ctx->search_results);
            ctx->search_results = calloc(ctx->clients_count, sizeof(int));
            ctx->search_results_count = 0;

            for (int i = 0; i < ctx->clients_count; i++)
            {
                if (strstr(ctx->clients[i].nom, ctx->field_values[0]) ||
                    strstr(ctx->clients[i].prenom, ctx->field_values[0]) ||
                    strstr(ctx->clients[i].email, ctx->field_values[0]))
                {
                    ctx->search_results[ctx->search_results_count++] = i;
                }
            }

            strcpy(ctx->status_message, "Search completed");
            ctx->status_type = 1; // Success
            ctx->status_timeout = 60;
        }

        ui_form_cleanup(ctx);
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
        return NAV_BACK;
    }

    ctx->needs_redraw = true;
    return NAV_NONE;
}

static void state_cleanup_clients_search(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_rooms(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    draw_content_window(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_rooms(UIContext *ctx, int key)
{
    // Handle Navigation
    NavDirection dir = ui_input_process_key(ctx, key);
    
    if (key == KEY_UP && ctx->selected_list_item > 0)
    {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset)
        {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }
    else if (key == KEY_DOWN && ctx->selected_list_item < ctx->chambres_count - 1)
    {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items)
        {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }
    else if (dir == NAV_BACK)
    {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 1; // Rooms menu item
        ctx->needs_redraw = true;
        return NAV_BACK;
    }



    return handle_rooms_input(ctx, key);
}

static void state_cleanup_rooms(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_rooms_add(UIContext *ctx)
{
    // Draw header and sidebar
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_footer(ctx, &layout);

    // Call the modal only when entering the state
    if (ctx->current_state != ctx->previous_state)
    {
        show_add_room_form(ctx->chambres, &ctx->chambres_count, ctx->chambres_capacity);
        // After modal, return to rooms list
        ctx->current_state = UI_STATE_ROOMS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
    }
}

static NavDirection state_handle_rooms_add(UIContext *ctx, int key)
{
    (void)key; /* Unused parameter */

    // Use the new refactored modal form function
    int result = show_add_room_form(ctx->chambres, &ctx->chambres_count, ctx->chambres_capacity);

    if (result == 0)
    {
        // Success
        strcpy(ctx->status_message, "Room added successfully");
        ctx->status_type = 1; // Success
        ctx->status_timeout = 60;
    }
    else
    {
        // Cancelled
        strcpy(ctx->status_message, "Room addition cancelled");
        ctx->status_type = 0; // Info
        ctx->status_timeout = 30;
    }

    // Return to rooms list
    ctx->current_state = UI_STATE_ROOMS;
    ctx->app_state = STATE_CONTENT_LIST;
    ctx->needs_redraw = true;
    return NAV_BACK;
}

static void state_cleanup_rooms_add(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_reservations(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    draw_content_window(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_reservations(UIContext *ctx, int key)
{
    return (NavDirection)handle_reservations_input(ctx, key);
}







static void state_cleanup_reservations(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_reservations_add(UIContext *ctx)
{
    // Draw header and sidebar
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_footer(ctx, &layout);

    // Call the modal only when entering the state
    if (ctx->current_state != ctx->previous_state)
    {
        show_reservation_wizard(ctx->chambres, ctx->chambres_count,
                                ctx->reservations, &ctx->reservations_count,
                                ctx->clients, ctx->clients_count);
        // After modal, return to reservations list
        ctx->current_state = UI_STATE_RESERVATIONS;
        ctx->app_state = STATE_CONTENT_LIST;
        ctx->needs_redraw = true;
    }
}

static NavDirection state_handle_reservations_add(UIContext *ctx, int key)
{
    (void)key; /* Unused parameter */
    // Modal is handled in draw function
    return NAV_NONE;
}

static void state_cleanup_reservations_add(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_billing(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    draw_content_window(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_billing(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    if (key == 'c' || key == 'C')
    {
        ctx->current_state = UI_STATE_BILLING_CREATE;
        ctx->needs_redraw = true;
    }
    else if (dir == NAV_BACK)
    {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }

    return dir;
}

static void state_cleanup_billing(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_billing_create(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_billing_create(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_billing_create(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == 27)
    {
        ctx->current_state = UI_STATE_BILLING;
        ctx->needs_redraw = true;
    }
    return dir;
}

static void state_cleanup_billing_create(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_help(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    draw_content_window(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_help(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == KEY_F(1) || key == KEY_EXIT || key == 27)
    {
        ctx->current_state = g_help_return_state;
        ctx->needs_redraw = true;
    }
    return dir;
}

static void state_cleanup_help(UIContext *ctx)
{
    (void)ctx;
}

static bool is_form_state(UIState state)
{
    return state == UI_STATE_CLIENTS_ADD || state == UI_STATE_CLIENTS_EDIT || state == UI_STATE_CLIENTS_SEARCH ||
           state == UI_STATE_ROOMS_ADD || state == UI_STATE_RESERVATIONS_ADD || state == UI_STATE_BILLING_CREATE;
}

/* Main UI functions */
bool ui_init(UIContext *ctx)
{
    if (!ctx)
    {
        LOG_ERROR("ui_init called with NULL ctx");
        return false;
    }

    g_ctx = ctx;
    LOG_DEBUG("UI init start (state=%d)", (int)ctx->current_state);

    /* Kan-initializiw ncurses */
    initscr();
    if (!stdscr)
    {
        LOG_ERROR("ncurses initialization failed: stdscr is NULL");
        return false;
    }

    /* Kanجيبّو taille dyal terminal */
    getmaxyx(stdscr, ctx->term_rows, ctx->term_cols);
    ui_context_resize(ctx, ctx->term_rows, ctx->term_cols);

    /* Setup dyal ncurses */
    cbreak();
    noecho();
    curs_set(0); /* Kan-khbbiw cursor */
    ui_input_init();
    ui_theme_init();

    /* Setup dyal resize handler */
#ifndef _WIN32
    signal(SIGWINCH, handle_resize);
#else
    /* F Windows, ghadi n-handliw resizing f main loop */
    SetConsoleCtrlHandler(NULL, FALSE);
#endif

    /* Create UI windows */
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    int y, x, h, w;
    ui_layout_get_header(&layout, &y, &x, &h, &w);
    ctx->header_win = newwin(h, w, y, x);

    ui_layout_get_sidebar(&layout, &y, &x, &h, &w);
    ctx->sidebar_win = newwin(h, w, y, x);

    ui_layout_get_content(&layout, &y, &x, &h, &w);
    ctx->content_win = newwin(h, w, y, x);

    ui_layout_get_footer(&layout, &y, &x, &h, &w);
    ctx->footer_win = newwin(h, w, y, x);

    /* Status line above footer */
    ctx->status_win = newwin(1, ctx->term_cols, y - 1, 0);

    /* Kan-chargiw data */
    ui_context_load_data(ctx);
    LOG_DEBUG("UI init complete (rows=%d cols=%d)", ctx->term_rows, ctx->term_cols);

    return true;
}

#ifdef _WIN32
static BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
        g_running = false;
        return TRUE;
    }
    return FALSE;
}
#endif

void ui_run(UIContext *ctx)
{
    if (!ctx)
    {
        LOG_ERROR("ui_run called with NULL ctx");
        return;
    }

    g_running = true;
    LOG_INFO("UI loop start");
    Layout layout;

#ifdef _WIN32
    /* Set up console handler f Windows */
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    /* Kanجيبّو taille l-oula dyal console */
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    ctx->term_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ctx->term_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    ui_context_resize(ctx, ctx->term_rows, ctx->term_cols);
#endif

    while (g_running && ctx->current_state != UI_STATE_EXIT)
    {
#ifdef _WIN32
        /* Kan-checkiw window resize f Windows */
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int new_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int new_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (new_cols != ctx->term_cols || new_rows != ctx->term_rows)
        {
            ctx->term_cols = new_cols;
            ctx->term_rows = new_rows;
            ui_context_resize(ctx, new_rows, new_cols);
            ctx->needs_redraw = true;
        }
#endif

        /* Kan7sbo layout */
        ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

        /* Double Buffered Drawing */
        if (ctx->needs_redraw || ctx->current_state != ctx->previous_state)
        {
            UIState state = ctx->current_state;
            if (state == UI_STATE_HELP && ctx->previous_state != UI_STATE_HELP)
            {
                g_help_return_state = ctx->previous_state;
            }

            /* Only clear if NOT a modal/form (preserves background) */
            if (!is_form_state(state))
            {
                erase();
            }

            /* ISOLATION: Do NOT draw dashboard if in a modal state */
            /* The state_handlers[state].draw() handles specific drawing */
            /* If we are in a form state, we assume the background is already there from previous draw */

            if (state < UI_STATE_COUNT && state_handlers[state].draw)
            {
                state_handlers[state].draw(ctx);
            }

            /* Add Footer/Guides (Context Aware) */
            ui_draw_footer(ctx, &layout); 
            
            /* Double Buffer Swap */
            wnoutrefresh(stdscr);
            doupdate();

            ctx->needs_redraw = false;
            ctx->previous_state = ctx->current_state;
        }

        /* Blocking Input - Zero CPU usage */
        int key = ui_input_get_key();

        if (key != ERR)
        {
            /* First handle app state navigation */
            NavDirection app_dir = handle_app_state_input(ctx, key);

            /* Then handle state-specific input if not handled by app state */
            if (app_dir == NAV_NONE)
            {
                UIState state = ctx->current_state;
                if (state < UI_STATE_COUNT && state_handlers[state].handle_input)
                {
                    NavDirection dir = state_handlers[state].handle_input(ctx, key);
                    if (dir == NAV_SELECT && ctx->current_state == UI_STATE_EXIT)
                    {
                        g_running = false;
                    }
                }
            }
            else if (app_dir == NAV_SELECT && ctx->current_state == UI_STATE_EXIT)
            {
                g_running = false;
            }
        }
    }

    /* Kan-sauvgiw data 9bel ma nkhrjo */
    ui_context_save_data(ctx);
    LOG_INFO("UI loop exit");
}

void ui_cleanup(UIContext *ctx)
{
    if (!ctx)
    {
        LOG_WARN("ui_cleanup called with NULL ctx");
        return;
    }

    /* Kan-nqaydo state l-haliya */
    UIState state = ctx->current_state;
    if (state < UI_STATE_COUNT && state_handlers[state].cleanup)
    {
        state_handlers[state].cleanup(ctx);
    }

    /* Delete UI windows */
    if (ctx->header_win)
        delwin(ctx->header_win);
    if (ctx->sidebar_win)
        delwin(ctx->sidebar_win);
    if (ctx->content_win)
        delwin(ctx->content_win);
    if (ctx->footer_win)
        delwin(ctx->footer_win);
    if (ctx->status_win)
        delwin(ctx->status_win);

    /* Kan-sddo ncurses */
    endwin();

    g_ctx = NULL;
    g_running = false;
}

void ui_redraw(UIContext *ctx)
{
    if (ctx)
    {
        ctx->needs_redraw = true;
    }
}

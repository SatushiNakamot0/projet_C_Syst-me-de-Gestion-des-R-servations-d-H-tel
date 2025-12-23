#include "ui.h"
#include "ui_state.h"
#include "ui_theme.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "ui_form.h"

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
#include <curses.h>
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

/* Declarations dyal state handlers */
static void state_draw_dashboard(UIContext *ctx);
static NavDirection state_handle_dashboard(UIContext *ctx, int key);
static void state_cleanup_dashboard(UIContext *ctx);

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
    ui_draw_clients_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_clients(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    /* Navigation f sidebar */
    if (dir == NAV_UP || dir == NAV_DOWN)
    {
        if (dir == NAV_UP && ctx->selected_menu_item > 0)
        {
            ctx->selected_menu_item--;
        }
        else if (dir == NAV_DOWN && ctx->selected_menu_item < 6)
        {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    /* Navigation f list */
    if (key == KEY_UP && ctx->selected_list_item > 0)
    {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset)
        {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    }
    else if (key == KEY_DOWN && ctx->selected_list_item < ctx->clients_count - 1)
    {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items)
        {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }

    /* Actions */
    if (key == 'a' || key == 'A')
    {
        ctx->current_state = UI_STATE_CLIENTS_ADD;
        ctx->needs_redraw = true;
    }
    else if (key == 'e' || key == 'E')
    {
        if (ctx->selected_list_item < ctx->clients_count)
        {
            ctx->current_state = UI_STATE_CLIENTS_EDIT;
            ctx->needs_redraw = true;
        }
    }
    else if (key == 'd' || key == 'D')
    {
        if (ctx->selected_list_item < ctx->clients_count)
        {
            ctx->current_state = UI_STATE_CLIENTS_DELETE;
            ctx->needs_redraw = true;
        }
    }
    else if (key == 's' || key == 'S')
    {
        ctx->current_state = UI_STATE_CLIENTS_SEARCH;
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
            if (ctx->clients_count < ctx->clients_capacity)
            {
                LOG_INFO("User submitted client form");
                LOG_DEBUG("Attempting to save client: %s %s, Email: %s, Tel: %s",
                          ctx->field_values[0], ctx->field_values[1],
                          ctx->field_values[2], ctx->field_values[3]);

                Client *new_client = &ctx->clients[ctx->clients_count];
                strncpy(new_client->nom, ctx->field_values[0], sizeof(new_client->nom) - 1);
                strncpy(new_client->prenom, ctx->field_values[1], sizeof(new_client->prenom) - 1);
                strncpy(new_client->email, ctx->field_values[2], sizeof(new_client->email) - 1);
                strncpy(new_client->telephone, ctx->field_values[3], sizeof(new_client->telephone) - 1);
                new_client->id = ctx->clients_count + 1; // Simple ID assignment

                // Directly add to array and save (bypass console-based ajouter_client)
                ctx->clients_count++;
                sauvegarder_clients(ctx->clients, ctx->clients_count);
                LOG_INFO("Client saved successfully to file");

                // Success
                strcpy(ctx->status_message, "Client added successfully");
                ctx->status_type = 1; // Success
                ctx->status_timeout = 60;
                ui_form_cleanup(ctx);
                ctx->current_state = UI_STATE_CLIENTS;
                ctx->needs_redraw = true;
                return NAV_BACK;
            }
            else
            {
                // Error - array is full
                LOG_ERROR("Cannot add client: database is full");
                strcpy(ctx->status_message, "Cannot add client: database is full");
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
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == 27)
    {
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->needs_redraw = true;
    }
    return dir;
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
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == 27)
    {
        ctx->current_state = UI_STATE_CLIENTS;
        ctx->needs_redraw = true;
    }
    return dir;
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
    ui_draw_rooms_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_rooms(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    /* Handle sidebar navigation */
    if (dir == NAV_UP || dir == NAV_DOWN)
    {
        if (dir == NAV_UP && ctx->selected_menu_item > 0)
        {
            ctx->selected_menu_item--;
        }
        else if (dir == NAV_DOWN && ctx->selected_menu_item < 6)
        {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    /* Handle list navigation */
    if (key == KEY_UP && ctx->selected_list_item > 0)
    {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset)
        {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    }
    else if (key == KEY_DOWN && ctx->selected_list_item < ctx->chambres_count - 1)
    {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items)
        {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }

    /* Handle actions */
    if (key == 'a' || key == 'A')
    {
        ctx->current_state = UI_STATE_ROOMS_ADD;
        ctx->needs_redraw = true;
    }
    else if (key == 'e' || key == 'E')
    {
        if (ctx->selected_list_item < ctx->chambres_count)
        {
            ctx->current_state = UI_STATE_ROOMS_EDIT;
            ctx->needs_redraw = true;
        }
    }
    else if (key == 'd' || key == 'D')
    {
        if (ctx->selected_list_item < ctx->chambres_count)
        {
            /* Delete room - khas dialog dyal confirmation */
            ctx->needs_redraw = true;
        }
    }
    else if (key == 's' || key == 'S')
    {
        /* Search rooms */
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

static void state_cleanup_rooms(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_rooms_add(UIContext *ctx)
{
    // Clear screen
    clear();

    // Draw header and sidebar
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);
    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_footer(ctx, &layout);

    // Create centered form window
    int form_height = 15;
    int form_width = 50;
    int start_y = (ctx->term_rows - form_height) / 2;
    int start_x = (ctx->term_cols - form_width) / 2;

    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    if (!form_win)
    {
        // Fallback to drawing on stdscr
        mvprintw(start_y, start_x, "Error: Could not create form window");
        refresh();
        return;
    }

    // Draw border
    box(form_win, 0, 0);

    // Title
    mvwprintw(form_win, 1, (form_width - 14) / 2, "ADD NEW ROOM");

    // Field labels and values
    const char *labels[] = {"Room Number:", "Type:", "Price:", "Save"};
    int num_fields = 4;

    for (int i = 0; i < num_fields; i++)
    {
        int field_y = 3 + i * 2;

        // Highlight current field
        if (i == ctx->current_field)
        {
            wattron(form_win, A_REVERSE);
            mvwprintw(form_win, field_y, 2, "%-12s", labels[i]);
            wattroff(form_win, A_REVERSE);
        }
        else
        {
            mvwprintw(form_win, field_y, 2, "%-12s", labels[i]);
        }

        // Draw field value
        if (i < 3)
        { // Input fields
            const char *value = (ctx->field_values && ctx->field_values[i]) ? ctx->field_values[i] : "";
            if (i == 1)
            { // Type field - show selector
                mvwprintw(form_win, field_y, 15, "[%-20s]", value);
                if (i == ctx->current_field)
                {
                    mvwprintw(form_win, field_y + 1, 15, "Use Left/Right arrows");
                }
            }
            else
            {
                mvwprintw(form_win, field_y, 15, "[%-20s]", value);
            }
        }
        else
        { // Save button
            if (i == ctx->current_field)
            {
                wattron(form_win, A_REVERSE);
                mvwprintw(form_win, field_y, 15, "[ Save ]");
                wattroff(form_win, A_REVERSE);
            }
            else
            {
                mvwprintw(form_win, field_y, 15, "[ Save ]");
            }
        }
    }

    // Instructions
    mvwprintw(form_win, form_height - 2, 2, "Arrows: Navigate | Enter: Select | ESC: Cancel");

    // Refresh windows
    wrefresh(form_win);
    refresh();

    // Clean up
    delwin(form_win);
}

static NavDirection state_handle_rooms_add(UIContext *ctx, int key)
{
    // Initialize form if first time
    if (!ctx->field_values)
    {
        ctx->num_fields = 4;
        ctx->current_field = 0;
        ctx->field_values = calloc(ctx->num_fields, sizeof(char *));
        for (int i = 0; i < ctx->num_fields; i++)
        {
            ctx->field_values[i] = strdup("");
        }
        // Default type
        strcpy(ctx->field_values[1], "Single");
        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    // Handle navigation
    if (key == KEY_UP)
    {
        if (ctx->current_field > 0)
        {
            ctx->current_field--;
            ctx->needs_redraw = true;
        }
        return NAV_NONE;
    }
    else if (key == KEY_DOWN)
    {
        if (ctx->current_field < 3)
        {
            ctx->current_field++;
            ctx->needs_redraw = true;
        }
        return NAV_NONE;
    }

    // Handle field-specific input
    if (ctx->current_field == 0 || ctx->current_field == 2)
    { // Room Number or Price - text input
        if (key == '\n' || key == KEY_ENTER)
        {
            // Move to next field
            if (ctx->current_field < 2)
            {
                ctx->current_field++;
            }
            else
            {
                // On Price, move to Save
                ctx->current_field = 3;
            }
            ctx->needs_redraw = true;
            return NAV_NONE;
        }
        else if (key == KEY_BACKSPACE || key == 127 || key == 8)
        {
            size_t len = strlen(ctx->field_values[ctx->current_field]);
            if (len > 0)
            {
                ctx->field_values[ctx->current_field][len - 1] = '\0';
                ctx->needs_redraw = true;
            }
        }
        else if (key >= 32 && key <= 126)
        {
            // For Price, only allow digits and .
            if (ctx->current_field == 2)
            {
                if (!((key >= '0' && key <= '9') || key == '.'))
                    return NAV_NONE;
            }
            size_t len = strlen(ctx->field_values[ctx->current_field]);
            if (len < 19) // Limit length
            {
                ctx->field_values[ctx->current_field][len] = (char)key;
                ctx->field_values[ctx->current_field][len + 1] = '\0';
                ctx->needs_redraw = true;
            }
        }
    }
    else if (ctx->current_field == 1)
    { // Type selector
        if (key == KEY_LEFT || key == KEY_RIGHT)
        {
            const char *types[] = {"Single", "Double", "Suite"};
            int current_index = 0;
            for (int i = 0; i < 3; i++)
            {
                if (strcmp(ctx->field_values[1], types[i]) == 0)
                {
                    current_index = i;
                    break;
                }
            }
            if (key == KEY_LEFT)
            {
                current_index = (current_index - 1 + 3) % 3;
            }
            else
            {
                current_index = (current_index + 1) % 3;
            }
            strcpy(ctx->field_values[1], types[current_index]);
            ctx->needs_redraw = true;
        }
        else if (key == '\n' || key == KEY_ENTER)
        {
            ctx->current_field = 2; // Move to Price
            ctx->needs_redraw = true;
        }
    }
    else if (ctx->current_field == 3)
    { // Save button
        if (key == '\n' || key == KEY_ENTER)
        {
            // Submit form
            if (strlen(ctx->field_values[0]) == 0 || strlen(ctx->field_values[1]) == 0 || strlen(ctx->field_values[2]) == 0)
            {
                strcpy(ctx->status_message, "All fields are required");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Validate room number
            int room_number = atoi(ctx->field_values[0]);
            if (room_number <= 0)
            {
                strcpy(ctx->status_message, "Room number must be a positive integer");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Check if room number already exists
            if (chambre_numero_existe(ctx->chambres, ctx->chambres_count, room_number))
            {
                strcpy(ctx->status_message, "Room number already exists");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Validate price
            float price = atof(ctx->field_values[2]);
            if (price <= 0.0f || price > 10000.0f)
            {
                strcpy(ctx->status_message, "Price must be between 0.01 and 10000.00");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Check if we have space for new room
            if (ctx->chambres_count >= ctx->chambres_capacity)
            {
                strcpy(ctx->status_message, "Cannot add room: database is full");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Add new room to the array
            LOG_INFO("User submitted room form");
            LOG_DEBUG("Attempting to save room: #%d %s (%.2f EUR)",
                      room_number, ctx->field_values[1], price);

            Chambre *new_room = &ctx->chambres[ctx->chambres_count];
            new_room->numero = room_number;
            strncpy(new_room->type, ctx->field_values[1], sizeof(new_room->type) - 1);
            new_room->prix = price;
            new_room->disponible = 1; // New rooms are available by default

            // Validate the room data
            if (!valider_chambre(new_room, ctx->chambres, ctx->chambres_count, -1))
            {
                strcpy(ctx->status_message, "Invalid room data");
                ctx->status_type = 3; // Error
                ctx->status_timeout = 60;
                ctx->needs_redraw = true;
                return NAV_NONE;
            }

            // Add to array and save
            ctx->chambres_count++;
            sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
            LOG_INFO("Room added successfully to file");

            // Success
            strcpy(ctx->status_message, "Room added successfully");
            ctx->status_type = 1; // Success
            ctx->status_timeout = 60;
            ui_form_cleanup(ctx);
            ctx->current_state = UI_STATE_ROOMS;
            ctx->needs_redraw = true;
            return NAV_BACK;
        }
    }

    // Cancel
    if (key == 27) // ESC
    {
        ui_form_cleanup(ctx);
        ctx->current_state = UI_STATE_ROOMS;
        ctx->needs_redraw = true;
        return NAV_BACK;
    }

    ctx->needs_redraw = true;
    return NAV_NONE;
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
    ui_draw_reservations_list(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_reservations(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);

    /* Handle sidebar navigation */
    if (dir == NAV_UP || dir == NAV_DOWN)
    {
        if (dir == NAV_UP && ctx->selected_menu_item > 0)
        {
            ctx->selected_menu_item--;
        }
        else if (dir == NAV_DOWN && ctx->selected_menu_item < 6)
        {
            ctx->selected_menu_item++;
        }
        ctx->needs_redraw = true;
        return NAV_NONE;
    }

    /* Handle list navigation */
    if (key == KEY_UP && ctx->selected_list_item > 0)
    {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset)
        {
            ctx->scroll_offset = ctx->selected_list_item;
        }
        ctx->needs_redraw = true;
    }
    else if (key == KEY_DOWN && ctx->selected_list_item < ctx->reservations_count - 1)
    {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items)
        {
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        }
        ctx->needs_redraw = true;
    }

    /* Handle actions */
    if (key == 'a' || key == 'A')
    {
        ctx->current_state = UI_STATE_RESERVATIONS_ADD;
        ctx->needs_redraw = true;
    }
    else if (key == 'e' || key == 'E')
    {
        if (ctx->selected_list_item < ctx->reservations_count)
        {
            ctx->current_state = UI_STATE_RESERVATIONS_EDIT;
            ctx->needs_redraw = true;
        }
    }
    else if (key == 'c' || key == 'C')
    {
        if (ctx->selected_list_item < ctx->reservations_count)
        {
            /* Cancel reservation - khas dialog dyal confirmation */
            ctx->needs_redraw = true;
        }
    }
    else if (dir == NAV_BACK)
    {
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 0;
        ctx->needs_redraw = true;
    }

    return dir;
}

static void state_cleanup_reservations(UIContext *ctx)
{
    (void)ctx;
}

static void state_draw_reservations_add(UIContext *ctx)
{
    Layout layout;
    ui_layout_calculate(&layout, ctx->term_rows, ctx->term_cols);

    ui_draw_header(ctx, &layout);
    ui_draw_sidebar(ctx, &layout);
    ui_draw_reservations_add(ctx, &layout);
    ui_draw_footer(ctx, &layout);
    ui_draw_status_message(ctx, &layout);
}

static NavDirection state_handle_reservations_add(UIContext *ctx, int key)
{
    NavDirection dir = ui_input_process_key(ctx, key);
    if (dir == NAV_BACK || key == 27)
    {
        ctx->current_state = UI_STATE_RESERVATIONS;
        ctx->needs_redraw = true;
    }
    return dir;
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
    ui_draw_billing_list(ctx, &layout);
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
    ui_draw_help(ctx, &layout);
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

        /* Kan-drawiw state l-haliya */
        if (ctx->needs_redraw || ctx->current_state != ctx->previous_state)
        {
            clear();

            UIState state = ctx->current_state;
            if (state == UI_STATE_HELP && ctx->previous_state != UI_STATE_HELP)
            {
                g_help_return_state = ctx->previous_state;
            }
            if (state < UI_STATE_COUNT && state_handlers[state].draw)
            {
                state_handlers[state].draw(ctx);
            }

            refresh();
            ctx->needs_redraw = false;
            ctx->previous_state = ctx->current_state;
        }

        /* Kan-traitiw input */
        int key = ui_input_get_key();
        if (key != ERR)
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

        /* Delay sghira bach ma ytl3ch CPU */
        napms(50); /* 50ms = ~20 FPS */
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

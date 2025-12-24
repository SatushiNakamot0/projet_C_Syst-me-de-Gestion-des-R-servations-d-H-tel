#include "ui_draw.h"
#include "ui_theme.h"
#include "ui_utils.h"
#include "../include/clients.h"
#include "../include/chambres.h"
#include "../include/reservations.h"
#include <string.h>
#include <stdio.h>
#include <ncurses.h>

/* ============================================================================
 * UI DRAWING IMPLEMENTATION
 * ============================================================================
 * Professional screen drawing functions with consistent styling.
 * ============================================================================ */

void ui_draw_header(UIContext *ctx, Layout *layout)
{
    (void)layout; /* Unused parameter */

    if (!ctx->header_win)
        return;

    static time_t last_update = 0;
    time_t now = time(NULL);
    
    // Only redraw header once per second to avoid flicker
    if (now == last_update) {
        return; // Skip redraw if same second
    }
    last_update = now;

    int h, w;
    getmaxyx(ctx->header_win, h, w);
    (void)h; /* Suppress unused variable warning */

    wattron(ctx->header_win, ui_theme_get_pair(COLOR_PAIR_HEADER));
    for (int i = 0; i < w; i++)
    {
        mvwaddch(ctx->header_win, 0, i, ' ');
    }

    /* Hotel name */
    mvwprintw(ctx->header_win, 0, 2, "HOTEL MANAGEMENT SYSTEM");

    /* Date and time */
    char date_str[32], time_str[32];
    ui_utils_get_current_date(date_str, sizeof(date_str));
    ui_utils_get_current_time(time_str, sizeof(time_str));
    mvwprintw(ctx->header_win, 0, w - 30, "%s | %s", date_str, time_str);

    wattroff(ctx->header_win, ui_theme_get_pair(COLOR_PAIR_HEADER));
    wrefresh(ctx->header_win);
}

/* Menu items for sidebar */
/* Dynamic Menu Items */
static const char *menu_admin[] = {
    "Dashboard", "Clients", "Rooms", "Reservations", "Billing", "Users [F2]", "Logout"
};
static const char *menu_receptionist[] = {
    "Dashboard", "Clients", "Rooms", "Reservations", "Billing", "Logout"
};
static const char *menu_client[] = {
    "Dashboard", "Book Room", "My Reservations", "Logout"
};

void ui_draw_sidebar(UIContext *ctx, Layout *layout)
{
    int y, x, h, w;
    ui_layout_get_sidebar(layout, &y, &x, &h, &w);

    /* Draw sidebar border - dim if not in sidebar focus */
    if (ctx->app_state != STATE_SIDEBAR)
    {
        attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    }
    ui_theme_draw_box(y, x, h, w);
    if (ctx->app_state != STATE_SIDEBAR)
    {
        attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    }

    /* Khtar menu 3la 7sab role (Select Menu) */
    const char **current_menu;
    int menu_count;

    if (strcmp(ctx->current_role, "client") == 0) {
        current_menu = menu_client;
        menu_count = 4;
    } else if (strcmp(ctx->current_role, "receptionist") == 0) {
        current_menu = menu_receptionist;
        menu_count = 6;
    } else { // Admin
        current_menu = menu_admin;
        menu_count = 7;
    }

    /* Rsm menu items (Draw Items) */
    int start_y = y + 2;
    // Hada l loop l jdid bach nsl7o l mochkil dyal l bleu (Bleeding Fix)
    for (int i = 0; i < menu_count && i < h - 4; i++)
    {
        int item_y = start_y + i;
        bool selected = (ctx->selected_menu_item == i);

        if (selected) {
            if (ctx->app_state == STATE_SIDEBAR)
                 wattron(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR_SELECTED)); // Cha3l l blue
            else 
                 wattron(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR)); // Or whatever dim style

            // %-18s kay3mmer l faragh b les espaces bach yji l loun m9ad
            mvprintw(item_y, x + 2, "%-18s", current_menu[i]);
            
            if (ctx->app_state == STATE_SIDEBAR)
                 wattroff(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR_SELECTED)); // Tfi l blue DGHYA!
            else
                 wattroff(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR));
        } else {
            // Ktb l menu 3adi bla loun
            if (ctx->app_state != STATE_SIDEBAR)
                wattron(stdscr, ui_theme_get_pair(COLOR_PAIR_DIM));
            else
                wattron(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR));

            mvprintw(item_y, x + 2, "%-18s", current_menu[i]);

            if (ctx->app_state != STATE_SIDEBAR)
                wattroff(stdscr, ui_theme_get_pair(COLOR_PAIR_DIM));
            else
                wattroff(stdscr, ui_theme_get_pair(COLOR_PAIR_SIDEBAR));
        }
    }
}

void ui_draw_footer(UIContext *ctx, Layout *layout)
{
    (void)ctx; /* Unused parameter */

    int y, x, h, w;
    ui_layout_get_footer(layout, &y, &x, &h, &w);

    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int i = 0; i < w; i++)
    {
        mvaddch(y, x + i, ' ');
    }

    /* Shortcuts - context sensitive */
    if (ctx->app_state == STATE_FORM_INPUT)
    {
        mvprintw(y, x + 2, "TAB/Shift+TAB: Next/Prev Field | Enter: Submit | ESC: Cancel");
    }
    else
    {
        mvprintw(y, x + 2, "Arrows: Navigate | Enter: Select | ESC: Back | Q: Quit | F1: Help");
    }

    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
}

void ui_draw_dashboard(UIContext *ctx, Layout *layout)
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

    /* Title */
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    ui_theme_draw_title("DASHBOARD", y, x, w);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    int content_start_y = y + 3;
    int current_y = content_start_y;

    /* KPI Cards */
    int card_width = (w - 4) / 2;
    int card_height = 8;

    /* Total Clients Card */
    ui_theme_draw_box(current_y, x + 1, card_height, card_width);
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(current_y + 1, x + 3, "Total Clients");
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));
    attron(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));
    mvprintw(current_y + 3, x + 3, "  %d  ", ctx->clients_count);
    attroff(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));
    mvprintw(current_y + 5, x + 3, ICON_CLIENT " Active accounts");

    /* Total Rooms Card */
    ui_theme_draw_box(current_y, x + card_width + 3, card_height, card_width);
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(current_y + 1, x + card_width + 5, "Total Rooms");
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));
    attron(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));
    mvprintw(current_y + 3, x + card_width + 5, "  %d  ", ctx->chambres_count);
    attroff(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));

    /* Calculate occupied rooms */
    int occupied_rooms = 0;
    for (int i = 0; i < ctx->reservations_count; i++)
    {
        /* Simple check - in real app, check dates */
        occupied_rooms++;
    }
    float occupancy_rate = ctx->chambres_count > 0 ? (occupied_rooms * 100.0f / ctx->chambres_count) : 0.0f;

    mvprintw(current_y + 5, x + card_width + 5, ICON_ROOM " Occupancy: %.1f%%", occupancy_rate);

    current_y += card_height + 2;

    /* Reservations Card */
    ui_theme_draw_box(current_y, x + 1, card_height, card_width);
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(current_y + 1, x + 3, "Reservations");
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));
    attron(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));
    mvprintw(current_y + 3, x + 3, "  %d  ", ctx->reservations_count);
    attroff(ui_theme_get_pair(COLOR_PAIR_HIGHLIGHT));
    mvprintw(current_y + 5, x + 3, ICON_RESERV " Active bookings");

    /* Billing Card */
    ui_theme_draw_box(current_y, x + card_width + 3, card_height, card_width);
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(current_y + 1, x + card_width + 5, "Total Revenue");
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    float total_revenue = 0.0f;
    for (int i = 0; i < ctx->factures_count; i++)
    {
        total_revenue += ctx->factures[i].total;
    }

    char revenue_str[32];
    ui_utils_format_currency(revenue_str, total_revenue, sizeof(revenue_str));
    attron(ui_theme_get_pair(COLOR_PAIR_SUCCESS));
    mvprintw(current_y + 3, x + card_width + 5, "  %s  ", revenue_str);
    attroff(ui_theme_get_pair(COLOR_PAIR_SUCCESS));
    mvprintw(current_y + 5, x + card_width + 5, ICON_BILL " Total invoices");

    current_y += card_height + 2;

    /* Occupancy Progress Bar */
    mvprintw(current_y, x + 1, "Overall Occupancy Rate:");
    ui_utils_draw_progress_bar(current_y + 1, x + 1, w - 2, occupancy_rate, NULL);
}

void ui_draw_clients_list(UIContext *ctx, Layout *layout)
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

    /* Title */
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    ui_theme_draw_title("CLIENTS MANAGEMENT", y, x, w);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    int content_start_y = y + 3;
    int table_y = content_start_y;

    /* Table headers */
    const char *headers[] = {"ID", "Nom", "Prenom", "Email", "Telephone"};
    int widths[] = {5, 20, 20, 30, 15};
    ui_utils_draw_table_header(table_y, x + 1, headers, widths, 5);

    /* Check if we have search results */
    int display_count = ctx->search_results_count > 0 ? ctx->search_results_count : ctx->clients_count;
    const int *display_indices = ctx->search_results_count > 0 ? ctx->search_results : NULL;

    /* Empty state handling */
    if (display_count == 0)
    {
        if (ctx->search_results_count > 0)
        {
            /* No search results */
            mvprintw(table_y + 4, x + 2, "No clients found matching your search.");
        }
        else
        {
            ui_utils_draw_empty_state(y, x, h, w, "clients", 'A');
        }
        /* Actions bar */
        int actions_y = y + h - 3;
        mvprintw(actions_y, x + 1, "[A]dd  [S]earch  [ESC]Back");
        return;
    }

    /* Show search info if applicable */
    if (ctx->search_results_count > 0)
    {
        attron(ui_theme_get_pair(COLOR_PAIR_DIM));
        mvprintw(table_y - 1, x + 1, "Search results: %d clients found", ctx->search_results_count);
        attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    }

    /* Table rows */
    int visible_rows = h - 6;
    int start_idx = ctx->scroll_offset;
    int end_idx = start_idx + visible_rows;
    if (end_idx > display_count)
        end_idx = display_count;

    for (int i = start_idx; i < end_idx; i++)
    {
        int row_y = table_y + 2 + (i - start_idx);
        bool selected = (ctx->selected_list_item == i);
        bool even = (i % 2 == 0);

        /* Get the actual client index */
        int client_idx = display_indices ? display_indices[i] : i;

        char id_str[16], nom_str[32], prenom_str[32], email_str[64], tel_str[32];
        snprintf(id_str, sizeof(id_str), "%d", ctx->clients[client_idx].id);
        strncpy(nom_str, ctx->clients[client_idx].nom, sizeof(nom_str));
        strncpy(prenom_str, ctx->clients[client_idx].prenom, sizeof(prenom_str));
        strncpy(email_str, ctx->clients[client_idx].email, sizeof(email_str));
        strncpy(tel_str, ctx->clients[client_idx].telephone, sizeof(tel_str));

        const char *values[] = {id_str, nom_str, prenom_str, email_str, tel_str};
        ui_utils_draw_table_row(row_y, x + 1, values, widths, 5, selected, even);
    }

    /* Actions bar */
    int actions_y = y + h - 3;
    mvprintw(actions_y, x + 1, "[A]dd  [E]dit  [D]elete  [S]earch  [ESC]Back");
}

void ui_draw_clients_add(UIContext *ctx, Layout *layout)
{
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    /* Clear and draw form */
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("ADD NEW CLIENT", y, x, w);

    int form_y = y + 3;

    // Draw fields
    const char *labels[] = {"Nom:", "Prenom:", "Email:", "Telephone:"};
    const int field_x = x + 4;
    const int field_width = 30;

    for (int i = 0; i < 4; i++)
    {
        // Highlight current field
        if (i == ctx->current_field)
        {
            attron(A_REVERSE);
            mvprintw(form_y + (i * 2), x + 2, ">");
            attroff(A_REVERSE);
        }

        // Draw label
        mvprintw(form_y + (i * 2), field_x, "%-10s", labels[i]);

        // Draw field
        const char *value = (i == ctx->current_field && ctx->in_input_mode) ? ctx->input_buffer : (ctx->field_values && ctx->field_values[i] ? ctx->field_values[i] : "");

        mvprintw(form_y + (i * 2), field_x + 10, "[%-*s]", field_width - 2, value);

        // Show cursor in input mode
        if (i == ctx->current_field && ctx->in_input_mode)
        {
            move(form_y + (i * 2), field_x + 11 + ctx->input_cursor_pos);
        }
    }

    // Instructions
    mvprintw(form_y + 9, x + 2, "Press ENTER to save, ESC to cancel");
}

void ui_draw_clients_edit(UIContext *ctx, Layout *layout)
{
    /* Similar to add but pre-filled */
    ui_draw_clients_add(ctx, layout);
    ui_theme_draw_title("EDIT CLIENT",
                        layout->content_y, layout->content_x, layout->content_width);
}

void ui_draw_clients_search(UIContext *ctx, Layout *layout)
{
    (void)ctx; /* Unused parameter */

    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("SEARCH CLIENTS", y, x, w);
    mvprintw(y + 3, x + 2, "Search: [                                        ]");
    mvprintw(y + 5, x + 2, "Results will appear here...");
}

void ui_draw_rooms_list(UIContext *ctx, Layout *layout)
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

    /* Title */
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    ui_theme_draw_title("ROOMS MANAGEMENT", y, x, w);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    int content_start_y = y + 3;
    int table_y = content_start_y;

    /* Table headers */
    const char *headers[] = {"Numero", "Type", "Prix/Nuit", "Disponible"};
    int widths[] = {8, 20, 12, 12};
    ui_utils_draw_table_header(table_y, x + 1, headers, widths, 4);

    /* Empty state handling */
    if (ctx->chambres_count == 0)
    {
        ui_utils_draw_empty_state(y, x, h, w, "rooms", 'A');
        /* Actions bar */
        int actions_y = y + h - 3;
        mvprintw(actions_y, x + 1, "[A]dd  [ESC]Back");
        return;
    }

    /* Table rows */
    int visible_rows = h - 6;
    int start_idx = ctx->scroll_offset;
    int end_idx = start_idx + visible_rows;
    if (end_idx > ctx->chambres_count)
        end_idx = ctx->chambres_count;

    for (int i = start_idx; i < end_idx; i++)
    {
        int row_y = table_y + 2 + (i - start_idx);
        bool selected = (ctx->selected_list_item == i);
        bool even = (i % 2 == 0);

        char numero_str[16], type_str[32], prix_str[32], dispo_str[16];
        snprintf(numero_str, sizeof(numero_str), "%d", ctx->chambres[i].numero);
        strncpy(type_str, ctx->chambres[i].type, sizeof(type_str));
        snprintf(prix_str, sizeof(prix_str), "%.2f EUR", ctx->chambres[i].prix);
        strncpy(dispo_str, ctx->chambres[i].disponible ? "Oui" : "Non", sizeof(dispo_str));

        const char *values[] = {numero_str, type_str, prix_str, dispo_str};
        ui_utils_draw_table_row(row_y, x + 1, values, widths, 4, selected, even);
    }

    /* Summary */
    int disponibles = 0;
    for (int i = 0; i < ctx->chambres_count; i++)
    {
        if (ctx->chambres[i].disponible)
            disponibles++;
    }

    int summary_y = y + h - 4;
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    mvprintw(summary_y, x + 1, "Total: %d chambres | Disponibles: %d | Occupees: %d",
             ctx->chambres_count, disponibles, ctx->chambres_count - disponibles);
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));

    /* Actions bar */
    int actions_y = y + h - 3;
    if (ctx->search_results_count > 0)
    {
        mvprintw(actions_y, x + 1, "[A]dd  [E]dit  [D]elete  [S]earch  [C]lear Search  [ESC]Back");
    }
    else
    {
        mvprintw(actions_y, x + 1, "[A]dd  [E]dit  [D]elete  [S]earch  [ESC]Back");
    }
}

void ui_draw_rooms_add(UIContext *ctx, Layout *layout)
{
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    /* Clear and draw form */
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("ADD NEW ROOM", y, x, w);

    int form_y = y + 3;

    // Draw fields
    const char *labels[] = {"Room Number:", "Type:", "Price/Night:"};
    const int field_x = x + 4;
    const int field_width = 30;

    for (int i = 0; i < 3; i++)
    {
        // Highlight current field
        if (i == ctx->current_field)
        {
            attron(A_REVERSE);
            mvprintw(form_y + (i * 2), x + 2, ">");
            attroff(A_REVERSE);
        }

        // Draw label
        mvprintw(form_y + (i * 2), field_x, "%-15s", labels[i]);

        // Draw field
        const char *value = (i == ctx->current_field && ctx->in_input_mode) ? ctx->input_buffer : (ctx->field_values && ctx->field_values[i] ? ctx->field_values[i] : "");

        if (i == 1)
        { // Room Type field - show selector
            mvprintw(form_y + (i * 2), field_x + 15, "[%-*s]", field_width - 2, value);
            mvprintw(form_y + (i * 2) + 1, field_x + 15, "Use Left/Right arrows to select");
        }
        else
        {
            mvprintw(form_y + (i * 2), field_x + 15, "[%-*s]", field_width - 2, value);
        }

        // Show cursor in input mode (only for text fields)
        if (i == ctx->current_field && ctx->in_input_mode && i != 1)
        {
            move(form_y + (i * 2), field_x + 16 + ctx->input_cursor_pos);
        }
    }

    // Instructions
    mvprintw(form_y + 9, x + 2, "Press ENTER to save, ESC to cancel");
}

void ui_draw_reservations_list(UIContext *ctx, Layout *layout)
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

    /* Title */
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    ui_theme_draw_title("RESERVATIONS", y, x, w);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    int content_start_y = y + 3;
    int table_y = content_start_y;

    /* Empty state handling */
    if (ctx->reservations_count == 0)
    {
        ui_utils_draw_empty_state(y, x, h, w, "reservations", 'A');
        /* Actions bar */
        int actions_y = y + h - 3;
        mvprintw(actions_y, x + 1, "[A]dd  [ESC]Back");
        return;
    }

    /* Table headers */
    const char *headers[] = {"ID", "Client", "Chambre", "Date Debut", "Date Fin", "Montant"};
    int widths[] = {5, 15, 8, 12, 12, 12};
    ui_utils_draw_table_header(table_y, x + 1, headers, widths, 6);

    /* Table rows */
    int visible_rows = h - 6;
    int start_idx = ctx->scroll_offset;
    int end_idx = start_idx + visible_rows;
    if (end_idx > ctx->reservations_count)
        end_idx = ctx->reservations_count;

    for (int i = start_idx; i < end_idx; i++)
    {
        int row_y = table_y + 2 + (i - start_idx);
        bool selected = (ctx->selected_list_item == i);
        bool even = (i % 2 == 0);

        /* Find client name */
        const char *nom_client = "Inconnu";
        for (int j = 0; j < ctx->clients_count; j++)
        {
            if (ctx->clients[j].id == ctx->reservations[i].client_id)
            {
                nom_client = ctx->clients[j].prenom;
                break;
            }
        }

        char id_str[16], client_str[32], chambre_str[16], date_debut_str[16], date_fin_str[16], montant_str[32];
        snprintf(id_str, sizeof(id_str), "%d", ctx->reservations[i].id);
        strncpy(client_str, nom_client, sizeof(client_str));
        snprintf(chambre_str, sizeof(chambre_str), "%d", ctx->reservations[i].chambre_numero);
        strncpy(date_debut_str, ctx->reservations[i].date_debut, sizeof(date_debut_str));
        strncpy(date_fin_str, ctx->reservations[i].date_fin, sizeof(date_fin_str));
        snprintf(montant_str, sizeof(montant_str), "%.2f EUR", ctx->reservations[i].montant);

        const char *values[] = {id_str, client_str, chambre_str, date_debut_str, date_fin_str, montant_str};
        ui_utils_draw_table_row(row_y, x + 1, values, widths, 6, selected, even);
    }

    /* Actions bar */
    int actions_y = y + h - 3;
    mvprintw(actions_y, x + 1, "[A]dd  [E]dit  [C]ancel  [ESC]Back");
}

void ui_draw_reservations_add(UIContext *ctx, Layout *layout)
{
    (void)ctx; /* Unused parameter */

    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("NEW RESERVATION", y, x, w);
    mvprintw(y + 3, x + 2, "Reservation form will be displayed here...");
}

void ui_draw_billing_list(UIContext *ctx, Layout *layout)
{
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("BILLING & INVOICES", y, x, w);

    int table_y = y + 3;
    const char *headers[] = {"ID", "Client ID", "Nuits", "Prix/Nuit", "Total"};
    int widths[] = {5, 10, 8, 12, 15};
    ui_utils_draw_table_header(table_y, x + 1, headers, widths, 5);

    int visible_rows = h - 6;
    int start_idx = ctx->scroll_offset;
    int end_idx = start_idx + visible_rows;
    if (end_idx > ctx->factures_count)
        end_idx = ctx->factures_count;

    for (int i = start_idx; i < end_idx; i++)
    {
        int row_y = table_y + 2 + (i - start_idx);
        bool selected = (ctx->selected_list_item == i);
        bool even = (i % 2 == 0);

        char id_str[16], client_id_str[16], nuits_str[16], prix_str[32], total_str[32];
        snprintf(id_str, sizeof(id_str), "%d", ctx->factures[i].idFacture);
        snprintf(client_id_str, sizeof(client_id_str), "%d", ctx->factures[i].idClient);
        snprintf(nuits_str, sizeof(nuits_str), "%d", ctx->factures[i].nbNuits);
        snprintf(prix_str, sizeof(prix_str), "%.2f", ctx->factures[i].prixNuit);
        ui_utils_format_currency(total_str, ctx->factures[i].total, sizeof(total_str));

        const char *values[] = {id_str, client_id_str, nuits_str, prix_str, total_str};
        ui_utils_draw_table_row(row_y, x + 1, values, widths, 5, selected, even);
    }

    mvprintw(y + h - 3, x + 1, "[C]reate  [V]iew  [ESC]Back");
}

void ui_draw_billing_create(UIContext *ctx, Layout *layout)
{
    (void)ctx; /* Unused parameter */

    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("CREATE INVOICE", y, x, w);
    mvprintw(y + 3, x + 2, "Invoice form will be displayed here...");
}

void ui_draw_help(UIContext *ctx, Layout *layout)
{
    (void)ctx; /* Unused parameter */

    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    ui_theme_draw_title("HELP & KEYBOARD SHORTCUTS", y, x, w);

    int help_y = y + 3;
    mvprintw(help_y, x + 2, "Navigation:");
    mvprintw(help_y + 1, x + 4, "Arrow Keys    - Navigate menus and lists");
    mvprintw(help_y + 2, x + 4, "Enter          - Select / Confirm");
    mvprintw(help_y + 3, x + 4, "ESC            - Go back / Cancel");
    mvprintw(help_y + 4, x + 4, "Q              - Quit application");

    mvprintw(help_y + 6, x + 2, "Actions:");
    mvprintw(help_y + 7, x + 4, "A              - Add new item");
    mvprintw(help_y + 8, x + 4, "E              - Edit selected item");
    mvprintw(help_y + 9, x + 4, "D              - Delete selected item");
    mvprintw(help_y + 10, x + 4, "S              - Search");
    mvprintw(help_y + 11, x + 4, "F1             - Show this help");

    mvprintw(help_y + 13, x + 2, "Press ESC to return to previous screen");
}

void ui_draw_dialog(UIContext *ctx)
{
    if (!ctx->dialog_active)
        return;

    int center_y = ctx->term_rows / 2;
    int center_x = ctx->term_cols / 2;
    int dialog_width = 50;
    int dialog_height = 8;

    int start_y = center_y - dialog_height / 2;
    int start_x = center_x - dialog_width / 2;

    /* Draw dialog box */
    ui_theme_draw_box(start_y, start_x, dialog_height, dialog_width);

    /* Title */
    attron(ui_theme_get_pair(COLOR_PAIR_TITLE));
    mvprintw(start_y + 1, start_x + 2, "%s", ctx->dialog_title);
    attroff(ui_theme_get_pair(COLOR_PAIR_TITLE));

    /* Message */
    mvprintw(start_y + 3, start_x + 2, "%s", ctx->dialog_message);

    /* Buttons */
    mvprintw(start_y + 5, start_x + dialog_width - 20, "[Yes]  [No]");
}

void ui_draw_status_message(UIContext *ctx, Layout *layout)
{
    if (ctx->status_timeout <= 0 || strlen(ctx->status_message) == 0)
    {
        return;
    }

    int y, x, h, w;
    ui_layout_get_footer(layout, &y, &x, &h, &w);

    int msg_y = y - 1;
    int msg_x = x + 2;

    ColorPair color_pair;
    switch (ctx->status_type)
    {
    case 1:
        color_pair = COLOR_PAIR_SUCCESS;
        break;
    case 2:
        color_pair = COLOR_PAIR_WARNING;
        break;
    case 3:
        color_pair = COLOR_PAIR_ERROR;
        break;
    default:
        color_pair = COLOR_PAIR_INFO;
        break;
    }

    attron(ui_theme_get_pair(color_pair));
    mvprintw(msg_y, msg_x, "%s", ctx->status_message);
    attroff(ui_theme_get_pair(color_pair));

    ctx->status_timeout--;
}

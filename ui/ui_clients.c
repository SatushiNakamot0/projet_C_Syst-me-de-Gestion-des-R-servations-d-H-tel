#include "ui_state.h"
#include "ui_clients.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/structures.h"
#include "../include/clients.h"
#include "../include/fichiers.h"
#include "ui_theme.h"
#include "ui_form.h"

/**
 * Shows search results in a selectable table
 * Returns selected client ID or -1 if cancelled
 */
int show_client_search_results(const char *search_term, Client clients[], int clients_count);

/**
 * Shows the client search modal
 * Returns selected client ID or -1 if cancelled
 */
int show_client_search_modal(Client clients[], int clients_count)
{
    noecho();
    cbreak();
    curs_set(1);

    // Dim background
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh();

    int height = 8, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    if (!win)
        return -1;

    char search_term[51] = "";
    int cursor_pos = 0;

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, (width - 15) / 2, "SEARCH CLIENT");

        mvwprintw(win, 3, 4, "Name: [%s]", search_term);
        mvwprintw(win, height - 2, 2, "Enter: Search | ESC: Cancel");
        wrefresh(win);

        int ch = getch();
        if (ch == 27)
        { // ESC
            delwin(win);
            echo();
            curs_set(0);
            clear();
            return -1;
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            if (strlen(search_term) == 0)
            {
                mvwprintw(win, 5, 4, "Search term required!");
                wrefresh(win);
                napms(1500);
                continue;
            }
            // Perform search and show results
            delwin(win);
            echo();
            curs_set(0);
            clear();
            return show_client_search_results(search_term, clients, clients_count);
        }
        else if (ch == KEY_BACKSPACE || ch == 127)
        {
            if (cursor_pos > 0)
            {
                search_term[--cursor_pos] = '\0';
            }
        }
        else if (isprint(ch) && strlen(search_term) < 50)
        {
            search_term[cursor_pos++] = ch;
            search_term[cursor_pos] = '\0';
        }
    }
}

/**
 * Shows search results in a selectable table
 * Returns selected client ID or -1 if cancelled
 */
int show_client_search_results(const char *search_term, Client clients[], int clients_count)
{
    // Filter clients by name (case-insensitive partial match)
    int matching_indices[MAX_CLIENTS];
    int match_count = 0;

    for (int i = 0; i < clients_count; i++)
    {
        char full_name[101];
        sprintf(full_name, "%s %s", clients[i].nom, clients[i].prenom);

        // Case-insensitive search
        char name_lower[101], term_lower[51];
        strcpy(name_lower, full_name);
        strcpy(term_lower, search_term);

        for (char *p = name_lower; *p; p++)
            *p = tolower(*p);
        for (char *p = term_lower; *p; p++)
            *p = tolower(*p);

        if (strstr(name_lower, term_lower))
        {
            matching_indices[match_count++] = i;
        }
    }

    if (match_count == 0)
    {
        // No matches - show message
        noecho();
        cbreak();
        curs_set(0);

        attron(ui_theme_get_pair(COLOR_PAIR_DIM));
        for (int y = 0; y < LINES; y++)
        {
            for (int x = 0; x < COLS; x++)
            {
                mvaddch(y, x, ' ');
            }
        }
        attroff(ui_theme_get_pair(COLOR_PAIR_DIM));

        int height = 6, width = 30;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;

        WINDOW *no_match_win = newwin(height, width, start_y, start_x);
        box(no_match_win, 0, 0);
        mvwprintw(no_match_win, 2, (width - 10) / 2, "NO MATCHES");
        wrefresh(no_match_win);
        napms(1500);
        delwin(no_match_win);
        clear();
        return -1;
    }

    // Show results table
    noecho();
    cbreak();
    curs_set(0);

    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh();

    int height = 15, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    if (!win)
        return -1;

    int selected_index = 0;
    int scroll_offset = 0;
    int max_visible = height - 4;

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, (width - 15) / 2, "SEARCH RESULTS");

        // Header
        mvwprintw(win, 3, 2, "%-5s %-20s %-25s %-15s", "ID", "Name", "Email", "Phone");

        // Display matching clients
        for (int i = 0; i < max_visible && (scroll_offset + i) < match_count; i++)
        {
            int client_idx = matching_indices[scroll_offset + i];
            Client *client = &clients[client_idx];

            if (i == selected_index)
            {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, 4 + i, 2, "%-5d %-20s %-25s %-15s",
                      client->id, client->nom, client->email, client->telephone);
            if (i == selected_index)
            {
                wattroff(win, A_REVERSE);
            }
        }

        mvwprintw(win, height - 2, 2, "Arrows: Navigate | Enter: Select | ESC: Back");
        wrefresh(win);

        int ch = getch();
        if (ch == 27)
        { // ESC
            delwin(win);
            clear();
            return -1;
        }
        else if (ch == KEY_UP && selected_index > 0)
        {
            selected_index--;
            if (selected_index < scroll_offset)
            {
                scroll_offset = selected_index;
            }
        }
        else if (ch == KEY_DOWN && selected_index < match_count - 1)
        {
            selected_index++;
            if (selected_index >= scroll_offset + max_visible)
            {
                scroll_offset = selected_index - max_visible + 1;
            }
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            int selected_client_id = clients[matching_indices[selected_index]].id;
            delwin(win);
            clear();
            return selected_client_id;
        }
    }
}

/**
 * Handles input for the clients list state
 * Incorporates Auto-Refresh Pattern: Save -> clear -> redraw
 */
int handle_clients_input(UIContext *ctx, int key)
{
    // Navigation
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) 
            ctx->scroll_offset = ctx->selected_list_item;
        ctx->needs_redraw = true;
        return NAV_NONE;
    } else if (key == KEY_DOWN) {
         int display_count = ctx->search_results_count > 0 ? ctx->search_results_count : ctx->clients_count;
         if (ctx->selected_list_item < display_count - 1) {
            ctx->selected_list_item++;
            if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) 
                ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
            ctx->needs_redraw = true;
         }
         return NAV_NONE;
    } else if (key == 27) { // ESC
        if (ctx->search_results_count > 0) {
             free(ctx->search_results);
             ctx->search_results = NULL;
             ctx->search_results_count = 0;
             ctx->selected_list_item = 0; 
             ctx->needs_redraw = true;
        } else {
             ctx->current_state = UI_STATE_DASHBOARD;
             ctx->selected_menu_item = 1; 
             ctx->needs_redraw = true;
             return NAV_BACK;
        }
        return NAV_NONE;
    }

    if (ctx->app_state == STATE_CONTENT_LIST)
    {
        int display_count = ctx->search_results_count > 0 ? ctx->search_results_count : ctx->clients_count;
        
        if (key == 'a' || key == 'A') {
             // ADD (State based)
             ctx->current_state = UI_STATE_CLIENTS_ADD;
             ctx->app_state = STATE_FORM_INPUT;
             ctx->needs_redraw = true;
        }
        else if (key == 'e' || key == 'E')
        {
             // EDIT (Modal)
             if (ctx->selected_list_item < display_count) {
                 int idx = ctx->search_results_count > 0 ? ctx->search_results[ctx->selected_list_item] : ctx->selected_list_item;
                 Client *client = &ctx->clients[idx];
                 
                 int res = show_edit_client_form(client, ctx->clients, ctx->clients_count);
                 
                 clear();
                 if (res == 0) { // Success
                     sauvegarder_clients(ctx->clients, ctx->clients_count);
                     strcpy(ctx->status_message, "Client Updated");
                     ctx->status_type=1;
                 }
                 ctx->needs_redraw = true;
             }
        }
        else if (key == 'd' || key == 'D') // Delete not supported by KEY_DC here? User said 'd' or 'Delete'.
        {
             // DELETE
             if (ctx->selected_list_item < display_count) {
                 int h = 8, w = 40;
                 WINDOW *popup = newwin(h, w, (LINES - h) / 2, (COLS - w) / 2);
                 wbkgd(popup, COLOR_PAIR(0));
                 box(popup, 0, 0);
                 
                 int idx = ctx->search_results_count > 0 ? ctx->search_results[ctx->selected_list_item] : ctx->selected_list_item;
                 int client_id = ctx->clients[idx].id;
                 
                 mvwprintw(popup, 2, 2, "Delete Client %d?", client_id);
                 mvwprintw(popup, 4, 10, "[Y] Yes   [N] No");
                 wrefresh(popup);
                 
                 int ch = 0;
                 while((ch = wgetch(popup)) != 'y' && ch != 'Y' && ch != 'n' && ch != 'N' && ch != 27);
                 delwin(popup);
                 
                 clear(); 
                 
                 if (ch == 'y' || ch == 'Y') {
                     if (client_supprimer(ctx->clients, &ctx->clients_count, client_id) == 0) {
                         sauvegarder_clients(ctx->clients, ctx->clients_count);
                         
                         // Reset search if active (safest)
                         if (ctx->search_results_count > 0) {
                             free(ctx->search_results);
                             ctx->search_results = NULL;
                             ctx->search_results_count = 0;
                         }

                         if (ctx->selected_list_item >= ctx->clients_count && ctx->selected_list_item > 0)
                             ctx->selected_list_item--;
                         
                         strcpy(ctx->status_message, "Client Deleted");
                         ctx->status_type=1;
                     } else {
                         strcpy(ctx->status_message, "Delete Failed");
                         ctx->status_type=2; 
                     }
                 }
                 ctx->needs_redraw = true;
             }
        }
        else if (key == 's' || key == 'S')
        {
            // Search
            int found_id = show_client_search_modal(ctx->clients, ctx->clients_count);
            clear();
            if (found_id != -1) {
                // Find
                for(int i=0; i<ctx->clients_count; i++) {
                    if (ctx->clients[i].id == found_id) {
                        ctx->selected_list_item = i;
                        // Scroll
                        if (i >= ctx->max_visible_items) ctx->scroll_offset = i - ctx->max_visible_items + 1;
                        else ctx->scroll_offset = 0;
                        break;
                    }
                }
            }
            ctx->needs_redraw = true;
        }
    }
    return NAV_NONE;
}

#include "ui_reservations.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "../include/structures.h"
#include "../include/reservations.h"
#include "../include/chambres.h"
#include "../include/clients.h"
#include "../include/fichiers.h"
#include "ui_theme.h"
#include "ui_clients.h"

// Reservation Wizard Context
typedef struct
{
    char date_debut[16];
    char date_fin[16];
    char type_chambre[2];
    int selected_room_id;
    int selected_client_id;
    int current_step; // 1=input, 2=selection, 3=client, 4=commit
} ReservationContext;

// Backend function implementations (simplified for this refactoring)
// Helper to find next available reservation ID
// Backend function logic moved to src/reservations.c

/**
 * Step 1: Inquiry Modal - Collect dates and room type
 */
int show_reservation_inquiry_modal(ReservationContext *ctx)
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

    int height = 12, width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    if (!win) return -1;

    keypad(win, TRUE); // Enable arrow keys

    const char *labels[] = {"Start Date:", "End Date:", "Room Type (S/D):"};
    char *fields[] = {ctx->date_debut, ctx->date_fin, ctx->type_chambre};
    int num_fields = 3;
    int current_field = 0;
    int cursor_pos = 0;

    // Initialize/clean buffers
    if (strlen(ctx->date_debut) == 0) strcpy(ctx->date_debut, "");
    if (strlen(ctx->date_fin) == 0) strcpy(ctx->date_fin, "");
    if (strlen(ctx->type_chambre) == 0) strcpy(ctx->type_chambre, "");

    /* Input Loop */
    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, (width - 19) / 2, "RESERVATION WIZARD");
        mvwprintw(win, 2, (width - 24) / 2, "Step 1: Dates & Context");

        for (int i = 0; i < num_fields; i++)
        {
            int field_y = 4 + (i * 2);
            
            // Draw label
            mvwprintw(win, field_y, 4, "%-20s", labels[i]);

            // Draw field background
            wattron(win, ui_theme_get_pair(COLOR_PAIR_INPUT));
            if (i == current_field) wattron(win, A_REVERSE);
            
            if (i < 2) { // Date fields
                mvwprintw(win, field_y, 25, "[%-10s]", fields[i]);
                // Draw mask hint if empty
                if (strlen(fields[i]) == 0 && i != current_field)
                     mvwprintw(win, field_y, 26, "DD/MM/YYYY");
            } else { // Room Type
                mvwprintw(win, field_y, 25, "[%-2s]", fields[i]);
            }
            
            if (i == current_field) wattroff(win, A_REVERSE);
            wattroff(win, ui_theme_get_pair(COLOR_PAIR_INPUT));
        }

        mvwprintw(win, height - 2, 2, "Arrows: Navigate | Enter: Next | ESC: Cancel");
        
        // Position Cursor
        int cursor_y = 4 + (current_field * 2);
        int cursor_x = 26 + cursor_pos;
        wmove(win, cursor_y, cursor_x);
        
        wrefresh(win);

        int ch = wgetch(win);
        
        if (ch == 27) { // ESC
            delwin(win);
            clear();
            return -1;
        }
        else if (ch == KEY_UP && current_field > 0) {
            current_field--;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == KEY_DOWN && current_field < num_fields - 1) {
            current_field++;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == '\n' || ch == KEY_ENTER) {
             // Validate
             if (strlen(ctx->date_debut) != 10 || strlen(ctx->date_fin) != 10 || strlen(ctx->type_chambre) == 0) {
                 mvwprintw(win, height - 3, 2, "Please complete all fields correctly!");
                 wrefresh(win);
                 napms(1000);
                 continue;
             }
             if (comparer_dates(ctx->date_debut, ctx->date_fin) >= 0) {
                 mvwprintw(win, height - 3, 2, "Start date must be before end date!");
                 wrefresh(win);
                 napms(1000);
                 continue;
             }
             delwin(win);
             clear();
             return 0; // Next step
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (cursor_pos > 0) {
                cursor_pos--;
                fields[current_field][cursor_pos] = '\0';
                // Skip slash backward
                if (current_field < 2 && (cursor_pos == 2 || cursor_pos == 5)) {
                     cursor_pos--;
                     fields[current_field][cursor_pos] = '\0';
                }
            }
        }
        else if (current_field < 2) { // Date Input
            if (isdigit(ch) && cursor_pos < 10) {
                // Auto-insert slash
                if (cursor_pos == 2 || cursor_pos == 5) {
                    fields[current_field][cursor_pos++] = '/';
                }
                
                if (cursor_pos < 10) {
                    fields[current_field][cursor_pos++] = ch;
                    fields[current_field][cursor_pos] = '\0';
                }
                
                // Auto-skip slash after typing
                if ((cursor_pos == 2 || cursor_pos == 5) && cursor_pos < 10) {
                     fields[current_field][cursor_pos++] = '/';
                     fields[current_field][cursor_pos] = '\0';
                }
            }
        }
        else { // Room Type
            if ((ch == 'S' || ch == 's' || ch == 'D' || ch == 'd') && cursor_pos < 1) {
                fields[current_field][cursor_pos++] = toupper(ch);
                fields[current_field][cursor_pos] = '\0';
            }
        }
    }
}

/**
 * Step 2: Visual Room Selection Modal
 */
int show_room_selection_modal(ReservationContext *ctx,
                              Chambre chambres[], int chambres_count,
                              Reservation reservations[], int reservations_count)
{
    // Filter rooms by type
    int matching_indices[MAX_CHAMBRES];
    int matching_count = 0;

    for (int i = 0; i < chambres_count; i++)
    {
        if (chambres[i].type[0] == ctx->type_chambre[0])
        {
            matching_indices[matching_count++] = i;
        }
    }

    if (matching_count == 0)
    {
        attron(ui_theme_get_pair(COLOR_PAIR_ERROR));
        mvprintw(LINES/2, (COLS-30)/2, "No rooms found of type %s", ctx->type_chambre);
        attroff(ui_theme_get_pair(COLOR_PAIR_ERROR));
        refresh();
        napms(1500);
        return -1;
    }

    noecho();
    cbreak();
    curs_set(0);

    // Dim background
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
        for (int x = 0; x < COLS; x++)
            mvaddch(y, x, ' ');
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh();

    int height = 18, width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    keypad(win, TRUE);

    int selected_index = 0;
    int scroll_offset = 0;
    int max_visible = height - 5;

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, (width - 25) / 2, "STEP 2: SELECT ROOM");
        mvwprintw(win, 2, 2, "Green = Available | Red = Occupied");

        // Header
        wattron(win, A_BOLD);
        mvwprintw(win, 3, 2, "%-8s %-15s %-10s %s", "Room #", "Type", "Price", "Status");
        wattroff(win, A_BOLD);
        mvwhline(win, 4, 1, ACS_HLINE, width - 2);

        // List
        for (int i = 0; i < max_visible && (scroll_offset + i) < matching_count; i++)
        {
            int room_idx = matching_indices[scroll_offset + i];
            Chambre *room = &chambres[room_idx];
            bool is_available = room->disponible && chambre_disponible_dates(reservations, reservations_count,
                                     room->numero, ctx->date_debut, ctx->date_fin, -1);
            
            int row_y = 5 + i;
            
            if (i + scroll_offset == selected_index) 
                wattron(win, A_REVERSE);
            
            // Color code status
            if (is_available)
                wattron(win, ui_theme_get_pair(COLOR_PAIR_SUCCESS));
            else
                wattron(win, ui_theme_get_pair(COLOR_PAIR_ERROR));

            mvwprintw(win, row_y, 2, "%-8d %-15s %-10.2f %s",
                      room->numero, room->type, room->prix, is_available ? "Available" : "Occupied");

            if (is_available)
                wattroff(win, ui_theme_get_pair(COLOR_PAIR_SUCCESS));
            else
                wattroff(win, ui_theme_get_pair(COLOR_PAIR_ERROR));
                
            if (i + scroll_offset == selected_index) 
                wattroff(win, A_REVERSE);
        }

        mvwprintw(win, height - 2, 2, "Arrows: Navigate | Enter: Select | ESC: Back");
        wrefresh(win);

        int ch = wgetch(win);
        if (ch == 27) { // ESC
            delwin(win);
            clear();
            return -1;
        }
        else if (ch == KEY_UP && selected_index > 0) {
            selected_index--;
            if (selected_index < scroll_offset) scroll_offset = selected_index;
        }
        else if (ch == KEY_DOWN && selected_index < matching_count - 1) {
            selected_index++;
            if (selected_index >= scroll_offset + max_visible) scroll_offset = selected_index - max_visible + 1;
        }
        else if (ch == '\n' || ch == KEY_ENTER) {
            // Select logic
            int room_idx = matching_indices[selected_index];
            Chambre *room = &chambres[room_idx];
            
            // Re-check availability
            if (room->disponible && chambre_disponible_dates(reservations, reservations_count,
                                     room->numero, ctx->date_debut, ctx->date_fin, -1)) {
                ctx->selected_room_id = room->numero;
                delwin(win);
                clear();
                return 0;
            } else {
                 mvwprintw(win, height - 3, 2, "Room is not available! Invalid selection.");
                 wrefresh(win);
                 napms(1000);
            }
        }
    }
}

/**
 * Step 3: Client Link Modal
 */
/**
 * Helper: Client Search Modal
 * Returns Selected Client ID or -1 if cancelled
 */
// function removed

/**
 * Step 3: Client Link Modal
 */
int show_client_link_modal(ReservationContext *ctx, Client clients[], int clients_count)
{
    noecho();
    cbreak();
    curs_set(1);

    // Dim background
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
        for (int x = 0; x < COLS; x++)
            mvaddch(y, x, ' ');
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh();

    int height = 10, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    if (!win) return -1;

    char client_id_str[16] = "";
    int cursor_pos = 0;

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, (width - 15) / 2, "STEP 3: CLIENT LINK");
        mvwprintw(win, 3, 4, "Enter Client ID directly below"); 
        mvwprintw(win, 4, 4, "OR Press 'F' to search by name");
        
        wattron(win, ui_theme_get_pair(COLOR_PAIR_INPUT));
        mvwprintw(win, 6, 4, "Client ID:");
        mvwprintw(win, 6, 16, "[%-10s]", client_id_str);
        wattroff(win, ui_theme_get_pair(COLOR_PAIR_INPUT));

        mvwprintw(win, height - 2, 2, "Enter: Confirm | F: Search | ESC: Cancel");
        
        // Cursor
        wmove(win, 6, 17 + cursor_pos);
        wrefresh(win);

        int ch = wgetch(win);
        
        if (ch == 27) { // ESC
            delwin(win);
            clear();
            return -1;
        }
        else if (ch == 'f' || ch == 'F') // Search Trigger
        {
            int found_id = show_client_search_modal(clients, clients_count);
            if (found_id != -1) {
                snprintf(client_id_str, sizeof(client_id_str), "%d", found_id);
                cursor_pos = strlen(client_id_str);
                
                // Redraw dim background as search modal might have messed it up
                attron(ui_theme_get_pair(COLOR_PAIR_DIM));
                for (int y = 0; y < LINES; y++)
                    for (int x = 0; x < COLS; x++)
                        mvaddch(y, x, ' ');
                attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
                refresh();
            }
            touchwin(win); // Mark window as needing redraw
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            if (strlen(client_id_str) == 0) {
                mvwprintw(win, 7, 4, "Client ID required!");
                wrefresh(win);
                napms(1000);
                continue;
            }
            int client_id = atoi(client_id_str);
            int found = 0;
            for (int i = 0; i < clients_count; i++) {
                if (clients[i].id == client_id) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                mvwprintw(win, 7, 4, "Client ID not found!");
                wrefresh(win);
                napms(1000);
                continue;
            }
            ctx->selected_client_id = client_id;
            delwin(win);
            clear();
            return 0;
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (cursor_pos > 0) {
                client_id_str[--cursor_pos] = '\0';
            }
        }
        else if (isdigit(ch) && cursor_pos < 10) {
            client_id_str[cursor_pos++] = ch;
            client_id_str[cursor_pos] = '\0';
        }
    }
}

/**
 * Main 3-Step Reservation Wizard
 */
int show_reservation_wizard(Chambre chambres[], int chambres_count,
                            Reservation reservations[], int *reservations_count,
                            Client clients[], int clients_count)
{
    ReservationContext ctx = {0};
    ctx.current_step = 1;

    // Step 1: Inquiry
    if (show_reservation_inquiry_modal(&ctx) != 0)
    {
        return -1; // Cancelled
    }
    ctx.current_step = 2;

    // Step 2: Room Selection
    if (show_room_selection_modal(&ctx, chambres, chambres_count, reservations, *reservations_count) != 0)
    {
        return -1; // Cancelled or no rooms
    }
    ctx.current_step = 3;

    // Step 3: Client Link
    if (show_client_link_modal(&ctx, clients, clients_count) != 0)
    {
        return -1; // Cancelled
    }
    ctx.current_step = 4;

    // Save reservation
    Reservation new_res;
    new_res.client_id = ctx.selected_client_id;
    new_res.chambre_numero = ctx.selected_room_id;
    strncpy(new_res.date_debut, ctx.date_debut, sizeof(new_res.date_debut) - 1);
    new_res.date_debut[sizeof(new_res.date_debut) - 1] = '\0';
    strncpy(new_res.date_fin, ctx.date_fin, sizeof(new_res.date_fin) - 1);
    new_res.date_fin[sizeof(new_res.date_fin) - 1] = '\0';

    if (reservation_ajouter(reservations, reservations_count, &new_res, chambres, chambres_count) == 0)
    {
        // Success message
        attron(ui_theme_get_pair(COLOR_PAIR_DIM));
        for (int y = 0; y < LINES; y++)
        {
            for (int x = 0; x < COLS; x++)
            {
                mvaddch(y, x, ' ');
            }
        }
        attroff(ui_theme_get_pair(COLOR_PAIR_DIM));

        int height = 6, width = 40;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;

        WINDOW *success_win = newwin(height, width, start_y, start_x);
        box(success_win, 0, 0);
        mvwprintw(success_win, 2, (width - 15) / 2, "RESERVATION SAVED");
        wrefresh(success_win);
        napms(1500);
        delwin(success_win);
        clear();
        return 0;
    }
    else
    {
        // Error message
        attron(ui_theme_get_pair(COLOR_PAIR_DIM));
        for (int y = 0; y < LINES; y++)
        {
            for (int x = 0; x < COLS; x++)
            {
                mvaddch(y, x, ' ');
            }
        }
        attroff(ui_theme_get_pair(COLOR_PAIR_DIM));

        int height = 6, width = 40;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;

        WINDOW *error_win = newwin(height, width, start_y, start_x);
        box(error_win, 0, 0);
        mvwprintw(error_win, 2, (width - 15) / 2, "SAVE FAILED");
        wrefresh(error_win);
        napms(1500);
        delwin(error_win);
        clear();
        return -1;
    }
}

/**
 * Billing Modal
 */
void show_billing_modal(Reservation *reservation, Client *client, Chambre *room)
{
    noecho();
    cbreak();
    curs_set(0);

    // Calculate billing info using mktime
    struct tm tm_start = {0};
    struct tm tm_end = {0};

    // Parse DD/MM/YYYY
    sscanf(reservation->date_debut, "%d/%d/%d", &tm_start.tm_mday, &tm_start.tm_mon, &tm_start.tm_year);
    sscanf(reservation->date_fin, "%d/%d/%d", &tm_end.tm_mday, &tm_end.tm_mon, &tm_end.tm_year);

    // Adjust for struct tm (Month 0-11, Year since 1900)
    tm_start.tm_mon -= 1; tm_start.tm_year -= 1900;
    tm_end.tm_mon -= 1;   tm_end.tm_year -= 1900;

    // Normalization (handles leap years etc)
    time_t t_start = mktime(&tm_start);
    time_t t_end = mktime(&tm_end);

    double seconds = difftime(t_end, t_start);
    int days = (int)(seconds / (24 * 3600));
    if (days < 1) days = 1;

    float total = days * room->prix;

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

    int height = 14, width = 55;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    box(win, 0, 0);
    mvwprintw(win, 1, (width - 10) / 2, " FACTURE ");

    mvwprintw(win, 3, 4, "Facture #: %d", reservation->id);
    mvwprintw(win, 4, 4, "Client   : %s %s", client->nom, client->prenom);
    mvwprintw(win, 5, 4, "Chambre  : %d (Prix: %.2f MAD/Nuit)", room->numero, room->prix);
    mvwprintw(win, 6, 4, "Periode  : %s -> %s", reservation->date_debut, reservation->date_fin);
    
    wattron(win, A_BOLD);
    mvwprintw(win, 8, 4, "Nuits    : %d", days);
    mvwprintw(win, 9, 4, "TOTAL    : %.2f MAD", total);
    wattroff(win, A_BOLD);

    mvwprintw(win, height - 2, 2, " [P] Print to File | [ESC] Close ");
    wrefresh(win);

    char msg[128] = {0};

    while (1)
    {
        if(strlen(msg)>0) mvwprintw(win, 11, 4, "Status: %s", msg);
        wrefresh(win);

        int ch = getch();
        if (ch == 27) // ESC
        { 
            break;
        }
        else if (ch == 'p' || ch == 'P')
        {
            char filename[64];
            snprintf(filename, sizeof(filename), "facture_%d.txt", reservation->id);
            FILE *fp = fopen(filename, "w");
            if (fp)
            {
                fprintf(fp, "=== FACTURE HOTEL ===\n");
                fprintf(fp, "ID Reservation: %d\n", reservation->id);
                fprintf(fp, "Client: %s %s\n", client->nom, client->prenom);
                fprintf(fp, "Chambre: %d\n", room->numero);
                fprintf(fp, "Dates: %s - %s\n", reservation->date_debut, reservation->date_fin);
                fprintf(fp, "Duree: %d Nuits\n", days);
                fprintf(fp, "Prix Unitaire: %.2f MAD\n", room->prix);
                fprintf(fp, "---------------------\n");
                fprintf(fp, "TOTAL: %.2f MAD\n", total);
                fprintf(fp, "=====================\n");
                fclose(fp);
                snprintf(msg, sizeof(msg), "Saved to %s!", filename);
            } else {
                strcpy(msg, "Error saving file!");
            }
        }
    }

    delwin(win);
    clear();
}

/* ============================================================================
 * EDIT RESERVATION LOGIC
 * ============================================================================
 */

int show_edit_reservation_form(Reservation *res, Reservation reservations[], int count, Chambre chambres[], int chambres_count)
{
    int height = 16;
    int width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *win = newwin(height, width, start_y, start_x);
    keypad(win, TRUE);
    wbkgd(win, COLOR_PAIR(0)); 
    box(win, 0, 0);
    
    mvwprintw(win, 1, (width-26)/2, " EDIT RESERVATION #%d ", res->id);
    mvwprintw(win, 3, 4, "Date Debut (DD/MM/YYYY):");
    mvwprintw(win, 6, 4, "Date Fin   (DD/MM/YYYY):");
    mvwprintw(win, 9, 4, "Chambre #              :");
    
    mvwprintw(win, height-3, 2, " [ENTER] Save   [ESC] Cancel ");
    
    char d_start[12], d_end[12], s_room[10];
    strcpy(d_start, res->date_debut);
    strcpy(d_end, res->date_fin);
    sprintf(s_room, "%d", res->chambre_numero);
    
    int field = 0; // 0=Start, 1=End
    char msg[64] = {0};
    
    while(1)
    {
        // Draw fields
        mvwprintw(win, 4, 4, "%s", d_start);
        mvwprintw(win, 7, 4, "%s", d_end);
        mvwprintw(win, 4, 28, field==0?"<--":"   ");
        mvwprintw(win, 7, 28, field==1?"<--":"   ");
        mvwprintw(win, 10, 4, "%s (Fixed)", s_room);

        if(strlen(msg)>0) {
            mvwprintw(win, 12, 4, "Msg: %s                  ", msg);
        } else {
             mvwprintw(win, 12, 4, "                         ");
        }

        wrefresh(win);
        
        int ch = wgetch(win);
        if(ch == 27) { // ESC
             delwin(win);
             return -1;
        }
        else if (ch == '\t' || ch == KEY_DOWN || ch == KEY_UP) {
            field = !field;
        }
        else if (ch == KEY_ENTER || ch == 10) {
            // Validation
            if(!valider_date(d_start) || !valider_date(d_end)) {
                strcpy(msg, "Invalid Date Format!");
                beep();
                continue;
            }
            if(comparer_dates(d_start, d_end) >= 0) {
                strcpy(msg, "End Date must be > Start!");
                beep();
                continue;
            }
            
            // Availability Check (exclude self)
            if(!chambre_disponible_dates(reservations, count, res->chambre_numero, d_start, d_end, res->id)) {
                 strcpy(msg, "Room Not Available!");
                 beep();
                 continue;
            }
            
            // Save
            Reservation temp = *res;
            strcpy(temp.date_debut, d_start);
            strcpy(temp.date_fin, d_end);
            
            if(reservation_modifier(reservations, count, &temp, chambres, chambres_count) == 0) {
                delwin(win);
                return 0;
            } else {
                strcpy(msg, "Save Failed!");
            }
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
             char *ptr = (field==0) ? d_start : d_end;
             int len = strlen(ptr);
             if(len > 0) ptr[len-1] = '\0';
             memset(msg, 0, sizeof(msg));
             // Redraw background of field to clear
             mvwprintw(win, field==0?4:7, 4, "                    "); 
        }
        else if (ch >= 32 && ch <= 126) {
             char *ptr = (field==0) ? d_start : d_end;
             int len = strlen(ptr);
             if(len < 10) {
                 ptr[len] = ch;
                 ptr[len+1] = '\0';
             }
             memset(msg, 0, sizeof(msg));
        }
    }
}

/* ============================================================================
 * LIST INPUT HANDLER
 * ============================================================================
 */
int handle_reservations_input(UIContext *ctx, int key)
{
    // Navigation
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) 
            ctx->scroll_offset = ctx->selected_list_item;
        ctx->needs_redraw = true;
        return NAV_NONE;
    } else if (key == KEY_DOWN && ctx->selected_list_item < ctx->reservations_count - 1) {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) 
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        ctx->needs_redraw = true;
        return NAV_NONE;
    } else if (key == 27) { // ESC
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 2; // Reservations
        ctx->needs_redraw = true;
        return NAV_BACK;
    }

    if (key == 'a' || key == 'A') {
         ctx->current_state = UI_STATE_RESERVATIONS_ADD;
         ctx->needs_redraw = true;
    }
    else if (key == 'c' || key == 'C') { // CANCEL
        if (ctx->selected_list_item < ctx->reservations_count) {
             Reservation *res = &ctx->reservations[ctx->selected_list_item];
             // Check if already cancelled
             if(strcmp(res->statut, "ANNULEE") == 0) { 
                 strcpy(ctx->status_message, "Already Cancelled");
                 ctx->status_type = 2;
                 ctx->needs_redraw = true;
                 return NAV_NONE;
             }
             
             // Confirmation Popup
             int h=8, w=40;
             WINDOW *popup = newwin(h, w, (LINES-h)/2, (COLS-w)/2);
             box(popup, 0, 0);
             mvwprintw(popup, 2, 2, "Cancel Reservation #%d?", res->id);
             mvwprintw(popup, 4, 10, "[Y] Yes   [N] No");
             wrefresh(popup);
             
             int ch;
             while((ch=wgetch(popup)) != 'y' && ch != 'Y' && ch != 'n' && ch != 'N' && ch != 27);
             delwin(popup);
             
             if(ch == 'y' || ch == 'Y') {
                 clear(); // Avoid artifacts
                 if(reservation_annuler(ctx->reservations, &ctx->reservations_count, res->id) == 0) {
                     strcpy(ctx->status_message, "Reservation Cancelled");
                     ctx->status_type=1;
                 }
             }
             clear();
             ctx->needs_redraw = true;
        }
    }
    else if (key == 'e' || key == 'E') { // EDIT
        if (ctx->selected_list_item < ctx->reservations_count) {
             Reservation *res = &ctx->reservations[ctx->selected_list_item];
             if(strcmp(res->statut, "ANNULEE") == 0) {
                 strcpy(ctx->status_message, "Cannot Edit Cancelled Res.");
                 ctx->status_type = 2;
                 ctx->needs_redraw = true;
                 return NAV_NONE;
             }
             
             int res_code = show_edit_reservation_form(res, ctx->reservations, ctx->reservations_count, ctx->chambres, ctx->chambres_count);
             clear();
             if(res_code == 0) {
                 strcpy(ctx->status_message, "Reservation Updated");
                 ctx->status_type=1;
             }
             ctx->needs_redraw = true;
        }
    }
    else if (key == 'b' || key == 'B') { // BILLING
         if (ctx->selected_list_item < ctx->reservations_count) {
             Reservation *res = &ctx->reservations[ctx->selected_list_item];
             // Find client/room helper
             Client *client = NULL;
             Chambre *room = NULL;
             for(int i=0; i<ctx->clients_count; i++) if(ctx->clients[i].id == res->client_id) client = &ctx->clients[i];
             for(int i=0; i<ctx->chambres_count; i++) if(ctx->chambres[i].numero == res->chambre_numero) room = &ctx->chambres[i];
             
             if(client && room) {
                 show_billing_modal(res, client, room);
                 clear();
             }
             ctx->needs_redraw = true;
         }
    }
    
    return NAV_NONE;
}
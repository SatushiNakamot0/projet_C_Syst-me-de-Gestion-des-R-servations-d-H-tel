#include "ui_rooms.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/structures.h"
#include "../include/chambres.h"
#include "../include/fichiers.h"
#include "ui_theme.h"

// Temporary buffer structure for room form data
typedef struct
{
    char numero[16];
    char type[20];
    char prix[16];
} RoomFormData;

/**
 * Shows and handles the add room form as a self-contained modal
 * Fixed rendering issues: follows safe rendering pattern
 */
int show_add_room_form(Chambre chambres[], int *count, int capacity)
{
    // Dim background for modal
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

    // Nuclear fix: Hardcoded safe dimensions
    int height = 20;
    int width = 60;
    int start_y = 5;
    int start_x = 10;

    WINDOW *form_win = newwin(height, width, start_y, start_x);
    if (!form_win)
    {
        return -1;
    }

    keypad(form_win, TRUE);
    wbkgd(form_win, COLOR_PAIR(1)); // Force known color pair
    noecho();
    cbreak();
    curs_set(1);

    // Temporary buffers for editing
    RoomFormData temp_data;
    strcpy(temp_data.numero, "");
    strcpy(temp_data.type, "Single"); // Default type
    strcpy(temp_data.prix, "");

    const char *labels[] = {"Room Number:", "Type:", "Price/Night:"};
    char *fields[] = {temp_data.numero, temp_data.type, temp_data.prix};
    int num_fields = 3;
    int current_field = 0;
    int cursor_pos = 0;

    int running = 1;
    while (running)
    {
        // Strict draw order
        werase(form_win);
        wattron(form_win, ui_theme_get_pair(COLOR_PAIR_ERROR));
        box(form_win, 0, 0);
        wattroff(form_win, ui_theme_get_pair(COLOR_PAIR_ERROR));
        mvwprintw(form_win, 1, 2, "ADD NEW ROOM (Debug Mode)");
        mvwprintw(form_win, 2, 2, "DEBUG: WINDOW IS VISIBLE");

        for (int i = 0; i < num_fields; i++)
        {
            int field_y = 3 + i;
            if (i == current_field)
            {
                wattron(form_win, A_REVERSE);
                mvwprintw(form_win, field_y, 2, ">");
                wattroff(form_win, A_REVERSE);
            }
            else
            {
                mvwprintw(form_win, field_y, 2, " ");
            }

            if (i == 1)
            { // Type field - show selector
                mvwprintw(form_win, field_y, 4, "%-15s [%s]", labels[i], fields[i]);
                mvwprintw(form_win, field_y + 1, 4, "Use Left/Right arrows to select");
            }
            else
            {
                mvwprintw(form_win, field_y, 4, "%-15s [%s]", labels[i], fields[i]);
            }

            // Position cursor
            if (i == current_field && i != 1)
            {
                wmove(form_win, field_y, 5 + strlen(labels[i]) + cursor_pos);
            }
        }
        mvwprintw(form_win, height - 2, 2, "Arrows: Navigate | Enter: Save | ESC: Cancel");

        // 4. Show to User
        wrefresh(form_win);

        // 5. Wait for input
        int ch = wgetch(form_win);

        // Handle input
        if (ch == 27) // ESC
        {
            running = 0;
        }
        else if (ch == KEY_UP && current_field > 0)
        {
            current_field--;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == KEY_DOWN && current_field < num_fields - 1)
        {
            current_field++;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            // Validate and save
            if (strlen(temp_data.numero) == 0 || strlen(temp_data.type) == 0 || strlen(temp_data.prix) == 0)
            {
                mvwprintw(form_win, height - 3, 2, "All fields are required!");
                wrefresh(form_win);
                napms(1500);
            }
            else
            {
                // Validate room number
                int room_number = atoi(temp_data.numero);
                if (room_number <= 0)
                {
                    mvwprintw(form_win, height - 3, 2, "Room number must be positive!");
                    wrefresh(form_win);
                    napms(1500);
                }
                else if (chambre_numero_existe(chambres, *count, room_number))
                {
                    mvwprintw(form_win, height - 3, 2, "Room number already exists!");
                    wrefresh(form_win);
                    napms(1500);
                }
                else
                {
                    // Validate price
                    float price = atof(temp_data.prix);
                    if (price <= 0.0f || price > 10000.0f)
                    {
                        mvwprintw(form_win, height - 3, 2, "Price must be 0.01-10000.00!");
                        wrefresh(form_win);
                        napms(1500);
                    }
                    else if (*count >= capacity)
                    {
                        mvwprintw(form_win, height - 3, 2, "Room database is full!");
                        wrefresh(form_win);
                        napms(1500);
                    }
                    else
                    {
                        // Create temp room object
                        Chambre new_room;
                        new_room.numero = room_number;
                        strncpy(new_room.type, temp_data.type, sizeof(new_room.type) - 1);
                        new_room.type[sizeof(new_room.type) - 1] = '\0';
                        new_room.prix = price;
                        new_room.disponible = 1;

                        if (chambre_ajouter(chambres, count, &new_room) == 0)
                        {
                            mvwprintw(form_win, height - 3, 2, "Room added successfully!");
                            wrefresh(form_win);
                            napms(1000);

                            running = 0;
                        }
                        else
                        {
                            mvwprintw(form_win, height - 3, 2, "Error adding room!");
                            wrefresh(form_win);
                            napms(1500);
                        }
                    }
                }
            }
        }
        else if (ch == KEY_BACKSPACE || ch == 127)
        {
            if (cursor_pos > 0)
            {
                memmove(&fields[current_field][cursor_pos - 1],
                        &fields[current_field][cursor_pos],
                        strlen(&fields[current_field][cursor_pos]) + 1);
                cursor_pos--;
            }
        }
        else if (current_field == 1 && (ch == KEY_LEFT || ch == KEY_RIGHT))
        { // Type selector
            const char *types[] = {"Single", "Double", "Suite"};
            int num_types = 3;
            int current_index = 0;

            for (int i = 0; i < num_types; i++)
            {
                if (strcmp(fields[1], types[i]) == 0)
                {
                    current_index = i;
                    break;
                }
            }

            if (ch == KEY_LEFT)
            {
                current_index = (current_index - 1 + num_types) % num_types;
            }
            else
            {
                current_index = (current_index + 1) % num_types;
            }
            strcpy(fields[1], types[current_index]);
        }
        else if (ch >= 32 && ch <= 126 && current_field != 1)
        { // Printable for text fields
            if (current_field == 2)
            { // Price field - allow digits and .
                if ((ch >= '0' && ch <= '9') || ch == '.')
                {
                    if (strlen(fields[current_field]) < 15)
                    {
                        memmove(&fields[current_field][cursor_pos + 1],
                                &fields[current_field][cursor_pos],
                                strlen(&fields[current_field][cursor_pos]) + 1);
                        fields[current_field][cursor_pos] = (char)ch;
                        cursor_pos++;
                    }
                }
            }
            else
            { // Room number - digits only
                if (ch >= '0' && ch <= '9')
                {
                    if (strlen(fields[current_field]) < 15)
                    {
                        memmove(&fields[current_field][cursor_pos + 1],
                                &fields[current_field][cursor_pos],
                                strlen(&fields[current_field][cursor_pos]) + 1);
                        fields[current_field][cursor_pos] = (char)ch;
                        cursor_pos++;
                    }
                }
            }
        }
    }

    // Cleanup
    delwin(form_win);
    curs_set(0);
    clear(); // Clear screen for proper return

    return running ? -1 : 0; // 0 on success, -1 on cancel
}

/**
 * Shows and handles the edit room form
 * Pre-fills data and allows editing Type and Price
 */
int show_edit_room_form(Chambre *room, Chambre chambres[], int count)
{
    // Dim background
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
        for (int x = 0; x < COLS; x++)
            mvaddch(y, x, ' ');
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh();

    // Dimensions
    int height = 20;
    int width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *form_win = newwin(height, width, start_y, start_x);
    if (!form_win) return -1;

    keypad(form_win, TRUE);
    wbkgd(form_win, COLOR_PAIR(1));
    noecho();
    cbreak();
    curs_set(1);

    // Pre-fill buffers
    RoomFormData temp_data;
    if (room != NULL) {
        snprintf(temp_data.numero, sizeof(temp_data.numero), "%d", room->numero);
        strncpy(temp_data.type, room->type, sizeof(temp_data.type)-1);
        temp_data.type[sizeof(temp_data.type)-1] = '\0';
        snprintf(temp_data.prix, sizeof(temp_data.prix), "%.2f", room->prix);
    } else {
        return -1;
    }

    const char *labels[] = {"Room Number:", "Type:", "Price/Night:"};
    char *fields[] = {temp_data.numero, temp_data.type, temp_data.prix};
    int num_fields = 3;
    int current_field = 1; // Start at Type (skip Number as it is ID)
    int cursor_pos = strlen(fields[current_field]);

    int running = 1;
    while (running)
    {
        werase(form_win);
        box(form_win, 0, 0);
        mvwprintw(form_win, 1, (width - 15) / 2, "EDIT ROOM");

        for (int i = 0; i < num_fields; i++)
        {
            int field_y = 4 + i * 3;
            if (i == current_field)
            {
                wattron(form_win, A_REVERSE);
                mvwprintw(form_win, field_y, 2, ">");
                wattroff(form_win, A_REVERSE);
            }

            mvwprintw(form_win, field_y, 4, "%s", labels[i]);
            
            // Draw input box styling
            wattron(form_win, ui_theme_get_pair(COLOR_PAIR_INPUT));
            if (i == 0) wattroff(form_win, ui_theme_get_pair(COLOR_PAIR_INPUT)); // No style for ID (readonly)
            
            mvwprintw(form_win, field_y + 1, 4, "[ %-20s ]", fields[i]);
            
            if (i == 0) wattron(form_win, ui_theme_get_pair(COLOR_PAIR_INPUT)); 
            wattroff(form_win, ui_theme_get_pair(COLOR_PAIR_INPUT));
            
            if (i == 0) mvwprintw(form_win, field_y + 1, 30, "(Read-only)");
            
            // Helpful hints
            if (i == 1 && current_field == 1) 
                mvwprintw(form_win, field_y + 2, 4, "Use Left/Right to change type");
        }
        
        mvwprintw(form_win, height - 2, 2, "Enter: Save | ESC: Cancel");
        
        // Cursor
        int field_y = 4 + current_field * 3;
        wmove(form_win, field_y + 1, 6 + cursor_pos);

        wrefresh(form_win);

        int ch = wgetch(form_win);

        if (ch == 27) { // ESC
            running = 0;
            delwin(form_win);
            clear();
            return -1;
        }
        else if (ch == KEY_UP && current_field > 1) { // Skip 0
            current_field--;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == KEY_DOWN && current_field < num_fields - 1) {
            current_field++;
            cursor_pos = strlen(fields[current_field]);
        }
        else if (ch == '\n' || ch == KEY_ENTER) {
             // Validate and Save
             float price = atof(temp_data.prix);
             if (price <= 0.0f || price > 10000.0f) {
                 mvwprintw(form_win, height - 3, 2, "Invalid Price!");
                 wrefresh(form_win);
                 napms(1000);
             } else {
                 // Update object
                 Chambre updated_room = *room;
                 strncpy(updated_room.type, temp_data.type, sizeof(updated_room.type)-1);
                 updated_room.prix = price;
                 
                 if (chambre_modifier(chambres, count, &updated_room) == 0) {
                     mvwprintw(form_win, height - 3, 2, "Room Updated!");
                     wrefresh(form_win);
                     napms(1000);
                     running = 0;
                     delwin(form_win);
                     clear();
                     return 0;
                 } else {
                     mvwprintw(form_win, height - 3, 2, "Update Failed!");
                     wrefresh(form_win);
                     napms(1000);
                 }
             }
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
             if (current_field != 0 && cursor_pos > 0) {
                 memmove(&fields[current_field][cursor_pos - 1], &fields[current_field][cursor_pos], strlen(&fields[current_field][cursor_pos]) + 1);
                 cursor_pos--;
             }
        }
        else if (current_field == 1 && (ch == KEY_LEFT || ch == KEY_RIGHT)) {
            // Type toggle
             const char *types[] = {"Single", "Double", "Suite"};
             int num_types = 3;
             int current_idx = 0;
             for(int i=0; i<num_types; i++) if(strcmp(fields[1], types[i])==0) current_idx = i;
             
             if(ch == KEY_LEFT) current_idx = (current_idx - 1 + num_types) % num_types;
             else current_idx = (current_idx + 1) % num_types;
             
             strcpy(fields[1], types[current_idx]);
             cursor_pos = strlen(fields[1]);
        }
        else if (current_field == 2 && ((ch >= '0' && ch <= '9') || ch == '.')) {
             if (strlen(fields[current_field]) < 10) {
                 memmove(&fields[current_field][cursor_pos + 1], &fields[current_field][cursor_pos], strlen(&fields[current_field][cursor_pos]) + 1);
                 fields[current_field][cursor_pos++] = ch;
             }
        }
    }
    return -1;
}

/**
 * Handles input for the rooms list state
 * Incorporates Auto-Refresh Pattern: Save -> clear -> redraw
 */
NavDirection handle_rooms_input(UIContext *ctx, int key)
{
    // Navigation
    if (key == KEY_UP && ctx->selected_list_item > 0) {
        ctx->selected_list_item--;
        if (ctx->selected_list_item < ctx->scroll_offset) 
            ctx->scroll_offset = ctx->selected_list_item;
        ctx->needs_redraw = true;
        return NAV_NONE;
    } else if (key == KEY_DOWN && ctx->selected_list_item < ctx->chambres_count - 1) {
        ctx->selected_list_item++;
        if (ctx->selected_list_item >= ctx->scroll_offset + ctx->max_visible_items) 
            ctx->scroll_offset = ctx->selected_list_item - ctx->max_visible_items + 1;
        ctx->needs_redraw = true;
        return NAV_NONE;
    } else if (key == 27) { // ESC
        ctx->current_state = UI_STATE_DASHBOARD;
        ctx->selected_menu_item = 1; // Rooms
        ctx->needs_redraw = true;
        return NAV_BACK;
    }

    if (ctx->app_state == STATE_CONTENT_LIST)
    {
        if (key == 'a' || key == 'A') {
             // ADD
             int res = show_add_room_form(ctx->chambres, &ctx->chambres_count, ctx->chambres_capacity);
             
             // FORCE RESET regardless of result to prevent black screen
             clear();
             if (res == 0) {
                 sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
                 strcpy(ctx->status_message, "Room Added");
                 ctx->status_type=1;
             }
             ctx->needs_redraw = true;
        }
        else if (key == 'e' || key == 'E')
        {
             // EDIT
             if (ctx->selected_list_item < ctx->chambres_count) {
                 Chambre *room = &ctx->chambres[ctx->selected_list_item];
                 int res = show_edit_room_form(room, ctx->chambres, ctx->chambres_count);
                 
                 clear(); // Fix empty page bug
                 if (res == 0) {
                     sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
                     strcpy(ctx->status_message, "Room Updated");
                     ctx->status_type=1;
                 }
                 ctx->needs_redraw = true;
             }
        }
        else if (key == 'd' || key == 'D')
        {
             // DELETE
             if (ctx->selected_list_item < ctx->chambres_count) {
                 int h = 8, w = 40;
                 WINDOW *popup = newwin(h, w, (LINES - h) / 2, (COLS - w) / 2);
                 wbkgd(popup, COLOR_PAIR(0));
                 box(popup, 0, 0);
                 mvwprintw(popup, 2, 2, "Delete Room %d?", ctx->chambres[ctx->selected_list_item].numero);
                 mvwprintw(popup, 4, 10, "[Y] Yes   [N] No");
                 wrefresh(popup);
                 
                 int ch = 0;
                 while((ch = wgetch(popup)) != 'y' && ch != 'Y' && ch != 'n' && ch != 'N' && ch != 27);
                 delwin(popup);
                 
                 clear(); // Clear before status update
                 
                 if (ch == 'y' || ch == 'Y') {
                     int num = ctx->chambres[ctx->selected_list_item].numero;
                     if (chambre_supprimer(ctx->chambres, &ctx->chambres_count, num) == 0) {
                         sauvegarder_chambres(ctx->chambres, ctx->chambres_count);
                         if (ctx->selected_list_item >= ctx->chambres_count && ctx->selected_list_item > 0)
                             ctx->selected_list_item--;
                         strcpy(ctx->status_message, "Room Deleted");
                         ctx->status_type=1;
                     } else {
                         strcpy(ctx->status_message, "Delete Failed");
                         ctx->status_type=2; // Error
                     }
                 }
                 ctx->needs_redraw = true;
             }
        }
    }
    return NAV_NONE;
}

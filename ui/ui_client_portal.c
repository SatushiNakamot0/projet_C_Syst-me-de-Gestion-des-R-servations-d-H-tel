#include "ui.h"
#include "ui_draw.h"
#include "ui_theme.h"
#include "ui_utils.h"
#include "../include/reservations.h"
#include "../include/clients.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

/* Helper to find current client ID based on username */
static int get_current_client_id(UIContext *ctx) {
    // Basic mapping: Check if username matches Last Name (Nom)
    for(int i=0; i<ctx->clients_count; i++) {
        if(strcmp(ctx->clients[i].nom, ctx->current_username) == 0) {
            return ctx->clients[i].id;
        }
    }
    return -1;
}

void ui_draw_client_dashboard(UIContext *ctx, Layout *layout) {
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);
    
    // Clear
    for(int i=0; i<h; i++) 
        for(int j=0; j<w; j++) 
            mvaddch(y+i, x+j, ' ');
            
    // Title
    ui_theme_draw_title("CLIENT DASHBOARD", y, x, w);
    
    // Welcome
    mvwprintw(stdscr, y+3, x+2, "Welcome back, %s!", ctx->current_username);
    
    int client_id = get_current_client_id(ctx);
    if(client_id == -1) {
        attron(COLOR_PAIR(3)); // Error
        mvwprintw(stdscr, y+5, x+2, "Warning: No Client Profile found for username '%s'.", ctx->current_username);
        mvwprintw(stdscr, y+6, x+2, "Please ask Reception to create a profile with Last Name = Username.");
        attroff(COLOR_PAIR(3));
    } else {
        // Count active reservations
        int active_res = 0;
        for(int i=0; i<ctx->reservations_count; i++) {
            if(ctx->reservations[i].client_id == client_id && strcmp(ctx->reservations[i].statut, "ACTIVE") == 0) {
                active_res++;
            }
        }
        
        mvwprintw(stdscr, y+5, x+2, "You have %d active reservations.", active_res);
    }
    
    mvwprintw(stdscr, y+8, x+2, "Use the sidebar to Book a Room or View Reservations.");
}

void ui_draw_client_my_reservations(UIContext *ctx, Layout *layout) {
    int y, x, h, w;
    ui_layout_get_content(layout, &y, &x, &h, &w);
    
     // Clear
    for(int i=0; i<h; i++) 
        for(int j=0; j<w; j++) 
            mvaddch(y+i, x+j, ' ');
            
    ui_theme_draw_title("MY RESERVATIONS", y, x, w);
    
    int client_id = get_current_client_id(ctx);
    if(client_id == -1) {
        mvwprintw(stdscr, y+3, x+2, "Profile not linked. Cannot show reservations.");
        return;
    }
    
    int table_y = y+3;
    const char *headers[] = {"ID", "Room", "Start", "End", "Status", "Total"};
    int widths[] = {5, 8, 12, 12, 10, 10};
    ui_utils_draw_table_header(table_y, x+1, headers, widths, 6);
    
    // List Filtering
    int my_indices[100];
    int count = 0;
    
    for(int i=0; i<ctx->reservations_count; i++) {
        if(ctx->reservations[i].client_id == client_id) {
            my_indices[count++] = i;
        }
    }
    
    int visible_rows = h - 6;
    int start_idx = ctx->scroll_offset;
    if(start_idx >= count && count > 0) start_idx = count - 1;
    if(start_idx < 0) start_idx = 0;
    
    int end_idx = start_idx + visible_rows;
    if(end_idx > count) end_idx = count;
    
    for(int i=start_idx; i<end_idx; i++) {
        int res_idx = my_indices[i];
        Reservation *res = &ctx->reservations[res_idx];
        
        int row_y = table_y + 2 + (i - start_idx);
        bool selected = (ctx->selected_list_item == i);
        bool even = (i % 2 == 0);
        
        char id_str[16], mm_str[16], start[16], end[16], status[16], total[16];
        snprintf(id_str, sizeof(id_str), "%d", res->id);
        snprintf(mm_str, sizeof(mm_str), "%d", res->chambre_numero);
        strcpy(start, res->date_debut);
        strcpy(end, res->date_fin);
        strncpy(status, res->statut, 15);
        snprintf(total, sizeof(total), "%.0f", res->montant);
        
        const char *vals[] = {id_str, mm_str, start, end, status, total};
        ui_utils_draw_table_row(row_y, x+1, vals, widths, 6, selected, even);
    }
    
    mvprintw(y+h-2, x+1, "[C]ancel Selected | [ESC] Back");
}

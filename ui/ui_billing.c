#include "ui_billing.h"
#include "ui_reservations.h"
#include "ui_theme.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

/**
 * Kanjbdo lfacturat mn dossiers (List invoices)
 */
static void billing_view_history(void);

/**
 * Kansawbo facture jdida (Create flow)
 */
static void billing_create_flow(UIContext *ctx);

/**
 * Modal bach nakhtaro reservation (Select reservation)
 */
static int select_reservation_modal(Reservation *reservations, int count, Client *clients, int clients_count);

/**
 * Menu dyal lfacturation (Blocking Menu)
 * Hada howa l-loop raissi dyal billing
 */
void show_billing_menu(UIContext *ctx) {
    bool running = true;
    while(running) {
        // Nms7o ecran (Standard clear)
        // clear(); // FIX: Don't clear global screen, keeps background visible
        
        // Nrsmo boite dyal menu f wast
        int h = 12, w = 50;
        int y = (LINES - h) / 2;
        int x = (COLS - w) / 2;
        
        WINDOW *win = newwin(h, w, y, x);
        box(win, 0, 0);
        mvwprintw(win, 2, (w-15)/2, "BILLING MANAGER");
        
        mvwprintw(win, 5, 10, "[C] Create New Invoice");
        mvwprintw(win, 7, 10, "[V] View History");
        mvwprintw(win, 9, 10, "[ESC] Back to Dashboard");
        
        wrefresh(win);
        
        // Ntsnaw l input
        int ch = getch();
        if (ch == 'c' || ch == 'C') {
            delwin(win);
            billing_create_flow(ctx);
        } else if (ch == 'v' || ch == 'V') {
            delwin(win);
            billing_view_history();
        } else if (ch == 27) { // ESC
             running = false;
        }
        
        delwin(win);
    }
}

static void billing_create_flow(UIContext *ctx) {
    // 1. Khtar reservation
    int res_idx = select_reservation_modal(ctx->reservations, ctx->reservations_count, ctx->clients, ctx->clients_count);
    
    if (res_idx >= 0) {
        Reservation *res = &ctx->reservations[res_idx];
        
        // Nl9aw client
        Client *client = NULL;
        for(int i=0; i<ctx->clients_count; i++) {
            if(ctx->clients[i].id == res->client_id) {
                client = &ctx->clients[i];
                break;
            }
        }
        
        // Nl9aw chambre
        Chambre *room = NULL;
        for(int i=0; i<ctx->chambres_count; i++) {
            if(ctx->chambres[i].numero == res->chambre_numero) {
                room = &ctx->chambres[i];
                break;
            }
        }
        
        if(client && room) {
             // N3ayto 3la modal dyal facture
             show_billing_modal(res, client, room);
        } else {
             // Mochkil
             attron(ui_theme_get_pair(COLOR_PAIR_ERROR));
             mvprintw(LINES-2, 2, "Error: Client or Room not found for this reservation!");
             attroff(ui_theme_get_pair(COLOR_PAIR_ERROR));
             getch();
        }
    }
}

static int select_reservation_modal(Reservation *reservations, int count, Client *clients, int clients_count) {
    int height = 20, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *win = newwin(height, width, start_y, start_x);
    keypad(win, TRUE);
    
    int selected = 0;
    int offset = 0;
    int max_vis = height - 5;
    
    while(1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "SELECT RESERVATION TO BILL");
        
        mvwprintw(win, 3, 2, "%-5s %-15s %-10s %-10s", "ID", "Client", "Room", "Status");
        mvwhline(win, 4, 2, ACS_HLINE, width-4);
        
        for(int i=0; i<max_vis && (i+offset) < count; i++) {
            int idx = i+offset;
            Reservation *r = &reservations[idx];
            
            // Njibo smiyt client
            char client_name[20] = "Unknown";
            for(int c=0; c<clients_count; c++) {
                if(clients[c].id == r->client_id) {
                    strncpy(client_name, clients[c].nom, 19);
                    break;
                }
            }
            
            // Highlight
            if(i+offset == selected) wattron(win, A_REVERSE);
            
            mvwprintw(win, 5+i, 2, "%-5d %-15s %-10d %-10s", 
                      r->id, client_name, r->chambre_numero, r->statut);
                      
            if(i+offset == selected) wattroff(win, A_REVERSE);
        }
        
        wrefresh(win);
        
        int ch = wgetch(win);
        if(ch == 27) { delwin(win); return -1; }
        else if(ch == KEY_UP && selected > 0) {
            selected--;
            if(selected < offset) offset = selected;
        }
        else if(ch == KEY_DOWN && selected < count - 1) {
            selected++;
            if(selected >= offset + max_vis) offset = selected - max_vis + 1;
        }
        else if(ch == '\n') {
            delwin(win);
            return selected;
        }
    }
}

static void billing_view_history(void) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    
    char files[100][100];
    int count = 0;
    
    // N9raw dossier
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strncmp(dir->d_name, "facture_", 8) == 0 && strstr(dir->d_name, ".txt")) {
                strncpy(files[count], dir->d_name, 99);
                count++;
                if(count >= 100) break;
            }
        }
        closedir(d);
    }
    
    // Ila ma l9ina walo
    if(count == 0) {
        WINDOW *msg = newwin(5, 40, (LINES-5)/2, (COLS-40)/2);
        box(msg, 0, 0);
        mvwprintw(msg, 2, 2, "No invoices found!");
        wrefresh(msg);
        napms(1500);
        delwin(msg);
        return;
    }
    
    // Select File Modal
    int h=15, w=50;
    WINDOW *win = newwin(h, w, (LINES-h)/2, (COLS-w)/2);
    keypad(win, TRUE);
    
    int sel=0, off=0, max_v=h-4;
    
    while(1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "SELECT INVOICE");
        
        for(int i=0; i<max_v && (i+off)<count; i++) {
             if(i+off == sel) wattron(win, A_REVERSE);
             mvwprintw(win, 3+i, 2, "%s", files[i+off]);
             if(i+off == sel) wattroff(win, A_REVERSE);
        }
        
        wrefresh(win);
        int ch = wgetch(win);
        if(ch == 27) { delwin(win); return; }
        else if(ch == KEY_UP && sel > 0) {
             sel--;
             if(sel<off) off=sel;
        }
        else if(ch == KEY_DOWN && sel < count-1) {
             sel++;
             if(sel>=off+max_v) off=sel-max_v+1;
        }
        else if(ch == '\n') {
             // Nft7o l fichier (Open File)
             char *fname = files[sel];
             FILE *fp = fopen(fname, "r");
             if(fp) {
                 // Content Viewer
                 WINDOW *vw = newwin(20, 60, (LINES-20)/2, (COLS-60)/2);
                 box(vw, 0, 0);
                 mvwprintw(vw, 1, 2, "VIEWING: %s", fname);
                 
                 char line[256];
                 int ly = 3;
                 while(fgets(line, sizeof(line), fp) && ly < 18) {
                     mvwprintw(vw, ly++, 2, "%s", line);
                 }
                 fclose(fp);
                 mvwprintw(vw, 18, 2, "Press Any Key to Close");
                 wrefresh(vw);
                 wgetch(vw);
                 delwin(vw);
             }
        }
    }
    delwin(win);
}

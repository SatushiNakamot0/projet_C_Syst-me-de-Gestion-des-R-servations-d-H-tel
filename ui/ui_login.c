#include "ui_login.h"
#include "ui_theme.h"
#include "ui_input.h"
#include <string.h>
#include <ncurses.h>
#include <ncurses.h>
#include "../include/auth.h"

// Forward declaration
static int read_password_masked(WINDOW *win, int y, int x, char *buffer, int max_len);

static void show_register_modal()
{
    int h=14, w=50;
    int starty = (LINES - h) / 2;
    int startx = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, starty, startx);
    keypad(win, TRUE);
    box(win, 0, 0);
    
    mvwprintw(win, 1, (w-10)/2, " REGISTER ");
    
    char u[32] = {0};
    char p[32] = {0};
    const char *roles[] = {"admin", "receptionist"};
    int role_idx = 1; // Default to receptionist
    
    int current_field = 0; // 0: User, 1: Pass, 2: Role, 3: Submit
    char msg[64] = {0};
    bool running = true;
    
    while(running)
    {
        // Draw Interface
        // Labels
        mvwprintw(win, 3, 4, "Username:");
        mvwprintw(win, 5, 4, "Password:");
        mvwprintw(win, 7, 4, "Role    :");
        
        // Fields Background
        mvwprintw(win, 3, 14, "[____________________]");
        mvwprintw(win, 5, 14, "[____________________]");
        mvwprintw(win, 7, 14, "[ < %-12s > ]", roles[role_idx]);
        
        // Draw Input Content
        if(current_field == 0) wattron(win, A_REVERSE);
        mvwprintw(win, 3, 15, "%-19s", u);
        wattroff(win, A_REVERSE);
        
        if(current_field == 1) wattron(win, A_REVERSE);
        char mask_p[32] = {0};
        for(size_t i=0; i<strlen(p); i++) mask_p[i] = '*';
        mvwprintw(win, 5, 15, "%-19s", mask_p);
        wattroff(win, A_REVERSE);
        
        // Highlight Role Label if active
        if(current_field == 2) mvwchgat(win, 7, 14, 20, A_REVERSE, 0, NULL);
        
        // Submit Button
        if(current_field == 3) wattron(win, A_REVERSE);
        mvwprintw(win, h-3, (w-10)/2, "[ SUBMIT ]");
        wattroff(win, A_REVERSE);
        
        // Status Msg
        if(strlen(msg) > 0) mvwprintw(win, h-5, 2, "%s", msg);
        
        mvwprintw(win, h-2, 2, "UP/DOWN: Navigate | ESC: Cancel");
        wrefresh(win);
        
        int ch = wgetch(win);
        if(ch == 27) { // ESC
            running = false;
        }
        else if(ch == KEY_DOWN || ch == 9) { // DOWN or TAB
            current_field++;
            if(current_field > 3) current_field = 0;
        }
        else if(ch == KEY_UP) {
            current_field--;
            if(current_field < 0) current_field = 3;
        }
        else if(ch == '\n' || ch == KEY_ENTER) {
            if(current_field < 3) current_field++;
            else {
                // Submit logic
                 if(strlen(u) > 0 && strlen(p) > 0) {
                    if(auth_register(u, p, roles[role_idx])) {
                        mvwprintw(win, h-5, 2, "Success! Press Any Key");
                        wrefresh(win);
                        wgetch(win);
                        running = false;
                    } else {
                        snprintf(msg, sizeof(msg), "Error: User exists!");
                        beep();
                    }
                } else {
                    snprintf(msg, sizeof(msg), "Error: Empty fields!");
                    beep();
                }
            }
        }
        else if(current_field == 0) {
            // Typing Username
            if(ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                int len = strlen(u);
                if(len > 0) u[len-1] = 0;
            } else if(ch >= 32 && ch <= 126 && strlen(u) < 19) {
                int len = strlen(u);
                u[len] = ch;
                u[len+1] = 0;
            }
        }
        else if(current_field == 1) {
             // Typing Password
            if(ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                int len = strlen(p);
                if(len > 0) p[len-1] = 0;
            } else if(ch >= 32 && ch <= 126 && strlen(p) < 19) {
                int len = strlen(p);
                p[len] = ch;
                p[len+1] = 0;
            }
        }
        else if(current_field == 2) {
             // Toggle Role
            if(ch == KEY_LEFT || ch == KEY_RIGHT) {
                role_idx = !role_idx; // Toggle 0 <-> 1
            }
        }
    }
    delwin(win);
    clear(); // Refresh parent
}

// Helper to read masked input
static int read_password_masked(WINDOW *win, int y, int x, char *buffer, int max_len)
{
    int pos = 0;
    int ch;
    
    // Clear buffer initially
    memset(buffer, 0, max_len);
    
    // Move to position
    wmove(win, y, x);
    wrefresh(win);
    
    curs_set(1); // Show cursor
    
    while(1)
    {
        ch = wgetch(win);
        
        if (ch == 10 || ch == 13 || ch == KEY_ENTER) // Enter
        {
            break;
        }
        else if (ch == 27) // ESC
        {
            curs_set(0);
            return 0;
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) // Backspace
        {
            if (pos > 0)
            {
                pos--;
                buffer[pos] = '\0';
                mvwprintw(win, y, x + pos, " "); // Erase *
                wmove(win, y, x + pos);
            }
        }
        else if (pos < max_len - 1 && ch >= 32 && ch <= 126) // Printable
        {
            buffer[pos] = ch;
            mvwaddch(win, y, x + pos, '*'); // Mask
            pos++;
            buffer[pos] = '\0';
        }
        wrefresh(win);
    }
    
    curs_set(0); // Hide cursor
    return 1;
}

bool show_login_screen(UIContext *ctx)
{
    (void)ctx; // Unused for now, but available if needed
    
    // Flush input buffer to prevent stray keystrokes from auto-cancelling
    flushinp();
    
    int height = 14;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    
    WINDOW *win = newwin(height, width, starty, startx);
    keypad(win, TRUE);
    wbkgd(win, COLOR_PAIR(0)); // Default background
    box(win, 0, 0);
    
    // Title
    const char *title = " LOGIN ";
    mvwprintw(win, 0, (width - strlen(title)) / 2, "%s", title);
    
    // Labels
    mvwprintw(win, 4, 5, "Username:");
    mvwprintw(win, 7, 5, "Password:");
    
    // Instructions
    // Instructions
    mvwprintw(win, height - 2, 2, " [ENTER] Login   [F2] Register ");
    mvwprintw(win, height - 4, (width - 25) / 2, "ESC to Exit");
    
    char username[32] = {0};
    char password[32] = {0};
    bool running = true;
    bool logged_in = false;
    
    char msg[64] = {0};
    
    while (running)
    {
        // Draw input boxes (visual only)
        mvwprintw(win, 4, 15, "[____________________]");
        mvwprintw(win, 7, 15, "[____________________]");
        
        // Fill boxes
        if (strlen(username) > 0) mvwprintw(win, 4, 16, "%s", username);
        // Password is masked, we don't print it here, handled by read function or manual masking if we were redrawing
        // But since we redraw the box, we should redraw the mask.
        for(size_t i=0; i<strlen(password); i++) mvwaddch(win, 7, 16 + i, '*');
        
        if (strlen(msg) > 0) {
            wattron(win, A_BOLD); // Maybe red color if available?
            mvwprintw(win, 10, (width - strlen(msg))/2, "%s", msg);
            wattroff(win, A_BOLD);
        }
        
        wrefresh(win);
        
        // Input Logic (Simple field selection)
        // We'll just force order: Username -> Password -> Submit
        
        // Read Username
        mvwprintw(win, 4, 16, "                    "); // Clear field area
        int res = ui_read_line(win, 4, 16, username, 20);
        if (res == 0) { // ESC
            running = false;
            break;
        } else if (res == 2) { // F2
            show_register_modal();
            // Redraw this win? Loop will redraw.
            box(win, 0, 0); // Redraw box in case modal cleared it
            mvwprintw(win, 0, (width - strlen(title)) / 2, "%s", title); // Redraw title
            continue;
        }
        
        // Read Password
        mvwprintw(win, 7, 16, "                    ");
        res = read_password_masked(win, 7, 16, password, 20);
        if (res == 0) { 
             if (strlen(password) == 0) { running = false; break; } 
        }
        
        // Validation
        if (auth_login(username, password, NULL)) {
            logged_in = true;
            running = false;
        } else {
            strcpy(msg, "Invalid Credentials!");
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            beep();
        }
    }
    
    delwin(win);
    clear();
    refresh();
    
    return logged_in;
}

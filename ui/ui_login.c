#include "ui_login.h"
#include "ui_theme.h"
#include "ui_input.h"
#include <string.h>
#include <ncurses.h>

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
    mvwprintw(win, height - 2, 2, " [ENTER] Login   [ESC] Exit ");
    mvwprintw(win, height - 4, (width - 25) / 2, "Default: admin / admin123");
    
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
        }
        
        // Read Password
        mvwprintw(win, 7, 16, "                    ");
        res = read_password_masked(win, 7, 16, password, 20);
        if (res == 0) { // ESC during password -> go back to start? or exit?
            // Let's exit for simplicity or loop back to username
            // User requested 'ESC' exits program in logic?
            // We'll loop back to start effectively.
             if (strlen(password) == 0) { running = false; break; } // If empty esc, exit
        }
        
        // Validation
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
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

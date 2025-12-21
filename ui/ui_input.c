#include "ui_input.h"
#include <ncurses.h>
#include <string.h>

/* ============================================================================
 * UI INPUT IMPLEMENTATION
 * ============================================================================
 * Processes keyboard input and converts to navigation actions.
 * Handles both navigation and form input modes.
 * ============================================================================ */

void ui_input_init(void) {
    /* Enable keypad for special keys */
    keypad(stdscr, TRUE);
    /* Enable non-blocking input */
    nodelay(stdscr, TRUE);
    /* Don't echo input */
    noecho();
    /* Enable function keys */
    meta(stdscr, TRUE);
}

int ui_input_get_key(void) {
    return getch();
}

bool ui_input_is_arrow_key(int key) {
    return (key == KEY_UP || key == KEY_DOWN || 
            key == KEY_LEFT || key == KEY_RIGHT);
}

NavDirection ui_input_arrow_to_direction(int key) {
    switch (key) {
        case KEY_UP:    return NAV_UP;
        case KEY_DOWN:  return NAV_DOWN;
        case KEY_LEFT:  return NAV_LEFT;
        case KEY_RIGHT: return NAV_RIGHT;
        default:        return NAV_NONE;
    }
}

void ui_input_process_form_input(UIContext *ctx, int key) {
    if (key == KEY_BACKSPACE || key == 127 || key == 8) {
        /* Backspace */
        if (ctx->input_cursor_pos > 0) {
            ctx->input_cursor_pos--;
            ctx->input_buffer[ctx->input_cursor_pos] = '\0';
        }
    } else if (key == KEY_LEFT) {
        if (ctx->input_cursor_pos > 0) {
            ctx->input_cursor_pos--;
        }
    } else if (key == KEY_RIGHT) {
        if (ctx->input_cursor_pos < (int)strlen(ctx->input_buffer)) {
            ctx->input_cursor_pos++;
        }
    } else if (key >= 32 && key <= 126) {
        /* Printable character */
        if (ctx->input_cursor_pos < (int)(sizeof(ctx->input_buffer) - 1)) {
            ctx->input_buffer[ctx->input_cursor_pos] = (char)key;
            ctx->input_cursor_pos++;
            ctx->input_buffer[ctx->input_cursor_pos] = '\0';
        }
    }
}

NavDirection ui_input_process_key(UIContext *ctx, int key) {
    /* Handle form input mode */
    if (ctx->input_mode == 1 || ctx->input_mode == 2) {
        if (key == 27) { /* ESC */
            ctx->input_mode = 0;
            ctx->input_buffer[0] = '\0';
            ctx->input_cursor_pos = 0;
            return NAV_BACK;
        } else {
            ui_input_process_form_input(ctx, key);
            return NAV_NONE;
        }
    }
    
    /* Normal navigation mode */
    switch (key) {
        case KEY_UP:
            return NAV_UP;
            
        case KEY_DOWN:
            return NAV_DOWN;
            
        case KEY_LEFT:
            return NAV_LEFT;
            
        case KEY_RIGHT:
            return NAV_RIGHT;
            
        case '\n':
        case KEY_ENTER:
        case ' ':
            return NAV_SELECT;
            
        case 27: /* ESC */
            return NAV_BACK;
            
        case 'q':
        case 'Q':
            if (ctx->current_state == UI_STATE_EXIT) {
                return NAV_SELECT;
            }
            /* Fall through to set exit state */
            ctx->current_state = UI_STATE_EXIT;
            return NAV_SELECT;
            
        case KEY_F(1):
            if (ctx->current_state != UI_STATE_HELP) {
                ctx->previous_state = ctx->current_state;
                ctx->current_state = UI_STATE_HELP;
                ctx->needs_redraw = true;
            }
            return NAV_NONE;
            
        /* Quick navigation shortcuts */
        case '1':
            ctx->current_state = UI_STATE_DASHBOARD;
            ctx->selected_menu_item = 0;
            ctx->needs_redraw = true;
            return NAV_NONE;
            
        case '2':
            ctx->current_state = UI_STATE_CLIENTS;
            ctx->selected_menu_item = 1;
            ctx->needs_redraw = true;
            return NAV_NONE;
            
        case '3':
            ctx->current_state = UI_STATE_ROOMS;
            ctx->selected_menu_item = 2;
            ctx->needs_redraw = true;
            return NAV_NONE;
            
        case '4':
            ctx->current_state = UI_STATE_RESERVATIONS;
            ctx->selected_menu_item = 3;
            ctx->needs_redraw = true;
            return NAV_NONE;
            
        case '5':
            ctx->current_state = UI_STATE_BILLING;
            ctx->selected_menu_item = 4;
            ctx->needs_redraw = true;
            return NAV_NONE;
            
        default:
            return NAV_NONE;
    }
}


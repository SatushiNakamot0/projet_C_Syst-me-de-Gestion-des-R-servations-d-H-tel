#include "ui_utils.h"
#include "ui_theme.h"
#include "ui_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <ncurses.h>
#include <stdbool.h>

/* ============================================================================
 * UI UTILITIES IMPLEMENTATION
 * ============================================================================ */

void ui_utils_trim_whitespace(char *str) {
    char *start = str;
    char *end;
    
    /* Trim leading whitespace */
    while (isspace((unsigned char)*start)) start++;
    
    /* Trim trailing whitespace */
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    
    /* Move trimmed string to beginning */
    memmove(str, start, strlen(start) + 1);
}

void ui_utils_center_string(char *dest, const char *src, int width) {
    int len = strlen(src);
    int padding = (width - len) / 2;
    if (padding < 0) padding = 0;
    
    memset(dest, ' ', width);
    memcpy(dest + padding, src, len < width ? len : width);
    dest[width] = '\0';
}

int ui_utils_string_width(const char *str) {
    int width = 0;
    while (*str) {
        if ((unsigned char)*str < 128) {
            width++;
        }
        str++;
    }
    return width;
}

void ui_utils_truncate_string(char *str, int max_width) {
    int len = strlen(str);
    if (len > max_width) {
        str[max_width - 3] = '.';
        str[max_width - 2] = '.';
        str[max_width - 1] = '.';
        str[max_width] = '\0';
    }
}

void ui_utils_get_current_date(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, buffer_size, "%d/%m/%Y", tm_info);
}

void ui_utils_get_current_time(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, buffer_size, "%H:%M:%S", tm_info);
}

void ui_utils_format_date(char *dest, const char *src) {
    /* Simple date formatter - assumes input is already in DD/MM/YYYY format */
    strncpy(dest, src, 11);
    dest[10] = '\0';
}

void ui_utils_draw_progress_bar(int y, int x, int width, float percentage, const char *label) {
    int bar_width = width - (label ? strlen(label) + 2 : 0);
    if (bar_width < 10) bar_width = 10;
    
    int filled = (int)(bar_width * percentage / 100.0f);
    if (filled > bar_width) filled = bar_width;
    if (filled < 0) filled = 0;
    
    int label_x = x;
    if (label) {
        mvprintw(y, x, "%s ", label);
        label_x += strlen(label) + 1;
    }
    
    /* Draw bar background */
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    mvaddch(y, label_x, '[');
    for (int i = 0; i < bar_width; i++) {
        mvaddch(y, label_x + 1 + i, '-');
    }
    mvaddch(y, label_x + 1 + bar_width, ']');
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    
    /* Draw filled portion */
    if (filled > 0) {
        attron(ui_theme_get_pair(COLOR_PAIR_SUCCESS));
        for (int i = 0; i < filled; i++) {
            mvaddch(y, label_x + 1 + i, '=');
        }
        attroff(ui_theme_get_pair(COLOR_PAIR_SUCCESS));
    }
    
    /* Draw percentage */
    char percent_str[16];
    snprintf(percent_str, sizeof(percent_str), " %.1f%%", percentage);
    mvprintw(y, label_x + 2 + bar_width, percent_str);
}

void ui_utils_draw_table_header(int y, int x, const char *headers[], int widths[], int count) {
    int current_x = x;
    attron(ui_theme_get_pair(COLOR_PAIR_TABLE_HEADER));
    
    for (int i = 0; i < count; i++) {
        mvprintw(y, current_x, "%-*s", widths[i], headers[i]);
        current_x += widths[i] + 1;
    }
    
    attroff(ui_theme_get_pair(COLOR_PAIR_TABLE_HEADER));
    
    /* Draw separator line */
    ui_theme_draw_hline(y + 1, x, current_x - x - 1);
}

void ui_utils_draw_table_row(int y, int x, const char *values[], int widths[], int count, bool selected, bool even) {
    int current_x = x;
    
    if (selected) {
        attron(ui_theme_get_pair(COLOR_PAIR_TABLE_SELECTED));
    } else if (even) {
        attron(ui_theme_get_pair(COLOR_PAIR_TABLE_EVEN));
    } else {
        attron(ui_theme_get_pair(COLOR_PAIR_TABLE_ODD));
    }
    
    for (int i = 0; i < count; i++) {
        char display[256];
        strncpy(display, values[i], widths[i]);
        display[widths[i]] = '\0';
        ui_utils_truncate_string(display, widths[i]);
        mvprintw(y, current_x, "%-*s", widths[i], display);
        current_x += widths[i] + 1;
    }
    
    if (selected) {
        attroff(ui_theme_get_pair(COLOR_PAIR_TABLE_SELECTED));
    } else if (even) {
        attroff(ui_theme_get_pair(COLOR_PAIR_TABLE_EVEN));
    } else {
        attroff(ui_theme_get_pair(COLOR_PAIR_TABLE_ODD));
    }
}

void ui_utils_init_pagination(Pagination *p, int total_items, int items_per_page) {
    p->total_items = total_items;
    p->items_per_page = items_per_page;
    p->total_pages = (total_items + items_per_page - 1) / items_per_page;
    if (p->total_pages == 0) p->total_pages = 1;
    p->current_page = 1;
}

void ui_utils_draw_pagination(int y, int x, Pagination *p) {
    char pagination_str[64];
    snprintf(pagination_str, sizeof(pagination_str), 
             "Page %d/%d (%d items)", 
             p->current_page, p->total_pages, p->total_items);
    mvprintw(y, x, pagination_str);
}

int ui_utils_get_page_start(Pagination *p) {
    return (p->current_page - 1) * p->items_per_page;
}

int ui_utils_get_page_end(Pagination *p) {
    int end = p->current_page * p->items_per_page;
    return end > p->total_items ? p->total_items : end;
}

bool ui_utils_validate_email(const char *email) {
    if (!email || strlen(email) == 0) return false;
    
    int at_count = 0;
    int dot_after_at = 0;
    const char *p = email;
    
    while (*p) {
        if (*p == '@') {
            at_count++;
            if (at_count > 1) return false;
        } else if (*p == '.' && at_count == 1) {
            dot_after_at = 1;
        }
        p++;
    }
    
    return at_count == 1 && dot_after_at == 1 && strlen(email) > 5;
}

bool ui_utils_validate_phone(const char *phone) {
    if (!phone) return false;
    int len = strlen(phone);
    if (len < 8 || len > 20) return false;
    
    const char *p = phone;
    int digit_count = 0;
    while (*p) {
        if (isdigit((unsigned char)*p)) {
            digit_count++;
        } else if (*p != '-' && *p != ' ' && *p != '+' && *p != '(' && *p != ')') {
            return false;
        }
        p++;
    }
    
    return digit_count >= 8;
}

bool ui_utils_validate_date(const char *date) {
    if (!date || strlen(date) != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;
    
    int day = atoi(date);
    int month = atoi(date + 3);
    int year = atoi(date + 6);
    
    if (day < 1 || day > 31) return false;
    if (month < 1 || month > 12) return false;
    if (year < 1900 || year > 2100) return false;
    
    return true;
}

void ui_utils_format_currency(char *dest, float amount, size_t dest_size) {
    snprintf(dest, dest_size, "%.2f EUR", amount);
}

void ui_utils_format_number(char *dest, int number, size_t dest_size) {
    snprintf(dest, dest_size, "%d", number);
}

void ui_utils_show_status(UIContext *ctx, int type, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(ctx->status_message, sizeof(ctx->status_message), format, args);
    va_end(args);
    
    ctx->status_type = type;
    ctx->status_timeout = ui_utils_get_status_timeout(type);
}

int ui_utils_get_status_timeout(int message_type) {
    /* Timeout in frames (at 20 FPS) */
    switch (message_type) {
        case 3: /* ERROR */
            return 160; /* 8 seconds */
        case 2: /* WARNING */
            return 120; /* 6 seconds */
        case 1: /* SUCCESS */
            return 80;  /* 4 seconds */
        case 0: /* INFO */
        default:
            return 60;  /* 3 seconds */
    }
}

void ui_utils_draw_empty_state(int y, int x, int h, int w, const char *item_name, char action_key) {
    int center_y = y + h / 2;
    int center_x = x + w / 2;
    
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    
    char msg[128];
    snprintf(msg, sizeof(msg), "No %s to display", item_name);
    int msg_len = strlen(msg);
    mvprintw(center_y, center_x - msg_len / 2, "%s", msg);
    
    if (action_key) {
        snprintf(msg, sizeof(msg), "Press [%c] to add a new %s", action_key, item_name);
        msg_len = strlen(msg);
        mvprintw(center_y + 2, center_x - msg_len / 2, "%s", msg);
    }
    
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
}

void ui_utils_format_error_message(char *dest, size_t dest_size, const char *type, 
                                   const char *what, const char *why, const char *how_to_fix) {
    if (!dest || dest_size == 0) return;
    
    if (why && how_to_fix) {
        snprintf(dest, dest_size, "%s: %s - %s - %s", type, what, why, how_to_fix);
    } else if (why) {
        snprintf(dest, dest_size, "%s: %s - %s", type, what, why);
    } else {
        snprintf(dest, dest_size, "%s: %s", type, what);
    }
}


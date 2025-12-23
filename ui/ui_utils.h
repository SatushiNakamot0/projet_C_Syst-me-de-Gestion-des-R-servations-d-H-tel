#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include "../include/structures.h"
#include "ui_state.h"  // For UIContext definition

/* ============================================================================
 * UI UTILITIES
 * ============================================================================
 * Helper functions for common UI operations: string formatting, date handling,
 * progress bars, pagination, etc.
 * ============================================================================ */

/* String utilities */
void ui_utils_trim_whitespace(char *str);
void ui_utils_center_string(char *dest, const char *src, int width);
int ui_utils_string_width(const char *str);
void ui_utils_truncate_string(char *str, int max_width);

/* Date utilities */
void ui_utils_get_current_date(char *buffer, size_t buffer_size);
void ui_utils_get_current_time(char *buffer, size_t buffer_size);
void ui_utils_format_date(char *dest, const char *src); /* Format: DD/MM/YYYY */

/* Progress bar drawing */
void ui_utils_draw_progress_bar(int y, int x, int width, float percentage, const char *label);

/* Table utilities */
void ui_utils_draw_table_header(int y, int x, const char *headers[], int widths[], int count);
void ui_utils_draw_table_row(int y, int x, const char *values[], int widths[], int count, bool selected, bool even);

/* Pagination */
typedef struct {
    int current_page;
    int total_pages;
    int items_per_page;
    int total_items;
} Pagination;

void ui_utils_init_pagination(Pagination *p, int total_items, int items_per_page);
void ui_utils_draw_pagination(int y, int x, Pagination *p);
int ui_utils_get_page_start(Pagination *p);
int ui_utils_get_page_end(Pagination *p);

/* Input validation */
bool ui_utils_validate_email(const char *email);
bool ui_utils_validate_phone(const char *phone);
bool ui_utils_validate_date(const char *date); /* Format: DD/MM/YYYY */

/* Number formatting */
void ui_utils_format_currency(char *dest, float amount, size_t dest_size);
void ui_utils_format_number(char *dest, int number, size_t dest_size);

/* Status message helpers */
void ui_utils_show_status(UIContext *ctx, int type, const char *format, ...);

/* Empty state drawing */
void ui_utils_draw_empty_state(int y, int x, int h, int w, const char *item_name, char action_key);

/* Error message formatting */
void ui_utils_format_error_message(char *dest, size_t dest_size, const char *type, 
                                   const char *what, const char *why, const char *how_to_fix);

/* Status message timeout calculation */
int ui_utils_get_status_timeout(int message_type);

#endif /* UI_UTILS_H */


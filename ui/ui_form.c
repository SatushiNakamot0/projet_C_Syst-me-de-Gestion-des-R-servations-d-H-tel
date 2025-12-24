#include "ui_form.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ncurses.h>
#include "../include/clients.h"
#include "../include/fichiers.h"
#include "../include/structures.h"
#include "ui_theme.h"

// Define field length constants
#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 20

void ui_form_start_input(UIContext *ctx, int mode)
{
    ctx->in_input_mode = true;
    ctx->input_mode = mode;
    ctx->input_cursor_pos = 0;
    ctx->input_buffer[0] = '\0';
    ctx->input_modified = false;

    // Enable cursor for input mode
    curs_set(1); // Show cursor
    noecho();    // Don't automatically echo - we'll handle it manually

    // Initialize field values if not already done
    if (ctx->field_values == NULL && ctx->num_fields > 0)
    {
        ctx->field_values = calloc(ctx->num_fields, sizeof(char *));
        for (int i = 0; i < ctx->num_fields; i++)
        {
            ctx->field_values[i] = strdup("");
        }
    }

    // Initialize selector for room type field (field 1 in rooms form)
    if (ctx->current_state == UI_STATE_ROOMS_ADD && ctx->current_field == 1)
    {
        if (strlen(ctx->field_values[1]) == 0)
        {
            strcpy(ctx->field_values[1], "Single"); // Default to Single
        }
    }
}

void ui_form_stop_input(UIContext *ctx)
{
    if (ctx->in_input_mode)
    {
        ctx->in_input_mode = false;
        ctx->input_mode = 0;
        ctx->input_cursor_pos = 0;
        ctx->input_buffer[0] = '\0';
        ctx->input_modified = false;

        // Disable cursor and echo when leaving input mode
        curs_set(0); // Hide cursor
        noecho();    // Don't echo typed characters
    }
}

int ui_form_handle_input(UIContext *ctx, int key)
{
    if (!ctx->in_input_mode)
        return FORM_CONTINUE;

    // Handle room type selector (field 1 in rooms add form)
    if (ctx->current_state == UI_STATE_ROOMS_ADD && ctx->current_field == 1)
    {
        if (key == KEY_LEFT || key == KEY_RIGHT)
        {
            const char *types[] = {"Single", "Double", "Suite"};
            int num_types = 3;
            int current_index = 0;

            // Find current type index
            for (int i = 0; i < num_types; i++)
            {
                if (strcmp(ctx->field_values[1], types[i]) == 0)
                {
                    current_index = i;
                    break;
                }
            }

            // Cycle through types
            if (key == KEY_LEFT)
            {
                current_index = (current_index - 1 + num_types) % num_types;
            }
            else if (key == KEY_RIGHT)
            {
                current_index = (current_index + 1) % num_types;
            }

            // Update values
            strcpy(ctx->field_values[1], types[current_index]);
            strcpy(ctx->input_buffer, types[current_index]);
            ctx->input_cursor_pos = strlen(ctx->input_buffer);
            ctx->input_modified = true;

            return FORM_CONTINUE;
        }
    }

    switch (key)
    {
    case KEY_UP:
        if (ctx->current_field > 0)
        {
            // Save current field
            if (ctx->field_values[ctx->current_field])
            {
                free(ctx->field_values[ctx->current_field]);
            }
            ctx->field_values[ctx->current_field] = strdup(ctx->input_buffer);

            // Move to previous field
            ctx->current_field--;
            strncpy(ctx->input_buffer,
                    ctx->field_values[ctx->current_field] ? ctx->field_values[ctx->current_field] : "",
                    sizeof(ctx->input_buffer) - 1);
            ctx->input_cursor_pos = strlen(ctx->input_buffer);
            ctx->input_modified = false;
        }
        return FORM_CONTINUE;

    case '\t': // TAB (forwards)
        // Save current field
        if (ctx->field_values[ctx->current_field])
        {
            free(ctx->field_values[ctx->current_field]);
        }
        ctx->field_values[ctx->current_field] = strdup(ctx->input_buffer);

        if (ctx->current_field < ctx->num_fields - 1)
        {
            // Move to next field
            ctx->current_field++;
            strncpy(ctx->input_buffer,
                    ctx->field_values[ctx->current_field] ? ctx->field_values[ctx->current_field] : "",
                    sizeof(ctx->input_buffer) - 1);
            ctx->input_cursor_pos = strlen(ctx->input_buffer);
            ctx->input_modified = false;
            return FORM_CONTINUE;
        }
        else
        {
            // Last field - submit form
            return FORM_SUBMIT;
        }

    case KEY_DOWN:
    case '\n':
    case KEY_ENTER:
        if (ctx->field_values[ctx->current_field])
        {
            free(ctx->field_values[ctx->current_field]);
        }
        ctx->field_values[ctx->current_field] = strdup(ctx->input_buffer);

        if (ctx->current_field < ctx->num_fields - 1)
        {
            // Move to next field
            ctx->current_field++;
            strncpy(ctx->input_buffer,
                    ctx->field_values[ctx->current_field] ? ctx->field_values[ctx->current_field] : "",
                    sizeof(ctx->input_buffer) - 1);
            ctx->input_cursor_pos = strlen(ctx->input_buffer);
            ctx->input_modified = false;
            return FORM_CONTINUE;
        }
        else
        {
            // Last field - submit form
            return FORM_SUBMIT;
        }

    case 27: // ESC
        ui_form_stop_input(ctx);
        return FORM_CANCEL;

    case KEY_BACKSPACE:
    case 127: // Backspace
        if (ctx->input_cursor_pos > 0)
        {
            memmove(&ctx->input_buffer[ctx->input_cursor_pos - 1],
                    &ctx->input_buffer[ctx->input_cursor_pos],
                    strlen(&ctx->input_buffer[ctx->input_cursor_pos]) + 1);
            ctx->input_cursor_pos--;
            ctx->input_modified = true;
        }
        return FORM_CONTINUE;

    default:
        if (key >= 32 && key <= 126)
        { // Printable characters
            if (strlen(ctx->input_buffer) < sizeof(ctx->input_buffer) - 1)
            {
                memmove(&ctx->input_buffer[ctx->input_cursor_pos + 1],
                        &ctx->input_buffer[ctx->input_cursor_pos],
                        strlen(&ctx->input_buffer[ctx->input_cursor_pos]) + 1);
                ctx->input_buffer[ctx->input_cursor_pos] = (char)key;
                ctx->input_cursor_pos++;
                ctx->input_modified = true;
            }
        }
        return FORM_CONTINUE;
    }

    return FORM_CONTINUE; // Default return
}

void ui_form_cleanup(UIContext *ctx)
{
    if (ctx->field_values)
    {
        for (int i = 0; i < ctx->num_fields; i++)
        {
            if (ctx->field_values[i])
            {
                free(ctx->field_values[i]);
            }
        }
        free(ctx->field_values);
        ctx->field_values = NULL;
    }
    ctx->num_fields = 0;
    ctx->current_field = 0;
    ui_form_stop_input(ctx);
    curs_set(0); // Ensure cursor is hidden when leaving forms
}

// show_add_reservation_form removed (replaced by wizard in ui_reservations.c)

// Temporary buffer structure for client form data
typedef struct
{
    char nom[MAX_NAME_LENGTH];
    char prenom[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char telephone[MAX_PHONE_LENGTH];
} ClientFormData;

/**
 * Shows and handles the edit client form as a self-contained modal
 * Fixes: Visual clutter (dimmed background), Double typing (echo management), Lag (optimized redraw)
 */
int show_edit_client_form(Client *client, Client clients[], int count)
{
    if (!client)
        return -1;

    // Step 2: DISABLE ECHO at the start to prevent double typing
    noecho();
    cbreak();
    curs_set(1);

    // Step 1: Draw dimmed background for clean modal appearance
    attron(ui_theme_get_pair(COLOR_PAIR_DIM));
    for (int y = 0; y < LINES; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(ui_theme_get_pair(COLOR_PAIR_DIM));
    refresh(); // Clear the entire screen with dim background

    // Create centered modal window
    int height = 12;
    int width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *form_win = newwin(height, width, start_y, start_x);
    if (!form_win)
    {
        curs_set(0);
        return -1;
    }

    // Temporary buffers for editing (buffered input approach)
    ClientFormData temp_data;
    strncpy(temp_data.nom, client->nom, sizeof(temp_data.nom) - 1);
    strncpy(temp_data.prenom, client->prenom, sizeof(temp_data.prenom) - 1);
    strncpy(temp_data.email, client->email, sizeof(temp_data.email) - 1);
    strncpy(temp_data.telephone, client->telephone, sizeof(temp_data.telephone) - 1);

    const char *labels[] = {"Nom:", "Prenom:", "Email:", "Telephone:"};
    char *fields[] = {temp_data.nom, temp_data.prenom, temp_data.email, temp_data.telephone};
    int num_fields = 4;
    int current_field = 0;
    int cursor_pos = 0;

    // Pre-loop rendering: Draw complete form with data
    werase(form_win);
    box(form_win, 0, 0);
    mvwprintw(form_win, 1, (width - 14) / 2, "EDIT CLIENT");

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
        mvwprintw(form_win, field_y, 4, "%-10s [%s]", labels[i], fields[i]);
    }
    mvwprintw(form_win, height - 2, 2, "Arrows: Navigate | Enter: Save | ESC: Cancel");
    wrefresh(form_win);

    // Buffered input loop
    int ch;
    while ((ch = getch()) != 27)
    { // ESC to cancel
        // Handle navigation
        if (ch == KEY_UP && current_field > 0)
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
            // Step 3: Save on ENTER
            Client modified_client = *client;
            strncpy(modified_client.nom, temp_data.nom, sizeof(modified_client.nom) - 1);
            modified_client.nom[sizeof(modified_client.nom) - 1] = '\0';
            strncpy(modified_client.prenom, temp_data.prenom, sizeof(modified_client.prenom) - 1);
            modified_client.prenom[sizeof(modified_client.prenom) - 1] = '\0';
            strncpy(modified_client.email, temp_data.email, sizeof(modified_client.email) - 1);
            modified_client.email[sizeof(modified_client.email) - 1] = '\0';
            strncpy(modified_client.telephone, temp_data.telephone, sizeof(modified_client.telephone) - 1);
            modified_client.telephone[sizeof(modified_client.telephone) - 1] = '\0';

            int res = client_modifier(clients, count, &modified_client);
            if (res == 0)
            {
                // Show success message
                mvwprintw(form_win, height - 3, 2, "Client updated successfully!");
                wrefresh(form_win);
                napms(1000); // Brief pause to show message

                delwin(form_win);
                curs_set(0);
                clear(); // Clear screen so previous screen redraws properly
                return 0;
            }
            else if (res == -2)
            {
                mvwprintw(form_win, height - 3, 2, "Error: Email already exists!");
                wrefresh(form_win);
                napms(1500);
                // Continue loop
            }
            else
            {
                mvwprintw(form_win, height - 3, 2, "Error updating client!");
                wrefresh(form_win);
                napms(1500);
                // Continue loop
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
        else if (ch >= 32 && ch <= 126)
        { // Printable characters
            // Step 2: BUFFERED INPUT - store in buffer, don't print immediately
            if (strlen(fields[current_field]) < MAX_NAME_LENGTH - 1)
            {
                memmove(&fields[current_field][cursor_pos + 1],
                        &fields[current_field][cursor_pos],
                        strlen(&fields[current_field][cursor_pos]) + 1);
                fields[current_field][cursor_pos] = (char)ch;
                cursor_pos++;
            }
        }

        // Step 2: OPTIMIZED REDRAW - clean slate each time
        werase(form_win);
        box(form_win, 0, 0);
        mvwprintw(form_win, 1, (width - 14) / 2, "EDIT CLIENT");

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
            // Print entire buffer nicely formatted
            mvwprintw(form_win, field_y, 4, "%-10s [%s]", labels[i], fields[i]);

            // Position cursor
            if (i == current_field)
            {
                wmove(form_win, field_y, 5 + strlen(labels[i]) + cursor_pos);
            }
        }
        mvwprintw(form_win, height - 2, 2, "Arrows: Navigate | Enter: Save | ESC: Cancel");
        wrefresh(form_win); // Only refresh the modal window
    }

    // Cancelled
    delwin(form_win);
    curs_set(0);
    clear(); // Clear screen for proper return
    return -1;
}

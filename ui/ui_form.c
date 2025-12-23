#include "ui_form.h"
#include <string.h>
#include <stdlib.h>
#include <curses.h>

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
}

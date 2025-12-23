#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "ui_state.h"
#include "ui_theme.h"
#include <stdbool.h>

/* ============================================================================
 * REUSABLE UI COMPONENTS
 * ============================================================================
 * Self-contained, reusable components with consistent behavior.
 * Each component manages its own state, drawing, and input handling.
 * ============================================================================ */

/* ============================================================================
 * TABLE COMPONENT
 * ============================================================================ */

typedef struct {
    int y, x;                    /* Position */
    int height, width;           /* Dimensions */
    const char **headers;        /* Column headers */
    int *col_widths;             /* Column widths */
    int col_count;               /* Number of columns */
    int selected_row;             /* Currently selected row */
    int scroll_offset;            /* Scroll position */
    int total_rows;              /* Total number of rows */
    bool has_focus;              /* Component has focus */
} TableComponent;

/* Initialize table component */
void ui_component_table_init(TableComponent *table, int y, int x, int h, int w,
                            const char **headers, int *widths, int cols);

/* Draw table */
void ui_component_table_draw(TableComponent *table, 
                            void (*row_drawer)(int row, int y, int x, void *data),
                            void *data);

/* Handle input */
bool ui_component_table_handle_input(TableComponent *table, int key);

/* Get selected row data index */
int ui_component_table_get_selected_index(const TableComponent *table);

/* ============================================================================
 * INPUT FIELD COMPONENT
 * ============================================================================ */

typedef struct {
    int y, x;                    /* Position */
    int width;                   /* Field width */
    char *buffer;                /* Input buffer */
    size_t buffer_size;          /* Buffer size */
    int cursor_pos;              /* Cursor position */
    const char *label;           /* Field label */
    const char *placeholder;     /* Placeholder text */
    bool has_focus;              /* Field has focus */
    bool is_password;            /* Password field (mask input) */
    bool is_error;               /* Validation error */
    const char *error_msg;       /* Error message */
} InputFieldComponent;

/* Initialize input field */
void ui_component_input_init(InputFieldComponent *field, int y, int x, int w,
                             char *buffer, size_t buf_size, const char *label);

/* Draw input field */
void ui_component_input_draw(const InputFieldComponent *field);

/* Handle input */
bool ui_component_input_handle_input(InputFieldComponent *field, int key);

/* Set focus */
void ui_component_input_set_focus(InputFieldComponent *field, bool focus);

/* Set error state */
void ui_component_input_set_error(InputFieldComponent *field, bool error, const char *msg);

/* ============================================================================
 * BUTTON COMPONENT
 * ============================================================================ */

typedef struct {
    int y, x;                    /* Position */
    const char *label;           /* Button label */
    char shortcut;               /* Shortcut key (0 = none) */
    bool has_focus;              /* Button has focus */
    bool is_default;             /* Default button (highlighted) */
    bool is_disabled;            /* Button disabled */
} ButtonComponent;

/* Initialize button */
void ui_component_button_init(ButtonComponent *btn, int y, int x, 
                              const char *label, char shortcut);

/* Draw button */
void ui_component_button_draw(const ButtonComponent *btn);

/* Handle input - returns true if activated */
bool ui_component_button_handle_input(ButtonComponent *btn, int key);

/* Set focus */
void ui_component_button_set_focus(ButtonComponent *btn, bool focus);

/* ============================================================================
 * STATUS MESSAGE COMPONENT
 * ============================================================================ */

typedef enum {
    STATUS_INFO = 0,
    STATUS_SUCCESS,
    STATUS_WARNING,
    STATUS_ERROR
} StatusType;

typedef struct {
    int y, x;                    /* Position */
    int width;                   /* Message width */
    char message[256];           /* Message text */
    StatusType type;             /* Message type */
    int timeout;                 /* Frames until auto-dismiss */
    bool visible;                /* Message visible */
} StatusMessageComponent;

/* Initialize status message */
void ui_component_status_init(StatusMessageComponent *status, int y, int x, int w);

/* Show message */
void ui_component_status_show(StatusMessageComponent *status, StatusType type, 
                              const char *format, ...);

/* Update (decrement timeout, hide if expired) */
void ui_component_status_update(StatusMessageComponent *status);

/* Draw status message */
void ui_component_status_draw(const StatusMessageComponent *status);

/* ============================================================================
 * MODAL DIALOG COMPONENT
 * ============================================================================ */

typedef struct {
    int center_y, center_x;      /* Dialog center position */
    int width, height;           /* Dialog dimensions */
    char title[64];             /* Dialog title */
    char message[256];           /* Dialog message */
    ButtonComponent *buttons;    /* Array of buttons */
    int button_count;            /* Number of buttons */
    int focused_button;          /* Currently focused button */
    bool visible;                /* Dialog visible */
} ModalDialogComponent;

/* Initialize modal dialog */
void ui_component_dialog_init(ModalDialogComponent *dialog, int w, int h);

/* Show dialog */
void ui_component_dialog_show(ModalDialogComponent *dialog, const char *title,
                             const char *message, ButtonComponent *buttons, int count);

/* Hide dialog */
void ui_component_dialog_hide(ModalDialogComponent *dialog);

/* Draw dialog */
void ui_component_dialog_draw(const ModalDialogComponent *dialog);

/* Handle input - returns button index if activated, -1 otherwise */
int ui_component_dialog_handle_input(ModalDialogComponent *dialog, int key);

/* ============================================================================
 * PROGRESS BAR COMPONENT
 * ============================================================================ */

typedef struct {
    int y, x;                    /* Position */
    int width;                   /* Bar width */
    float percentage;             /* Progress percentage (0-100) */
    const char *label;           /* Label text */
    bool show_percentage;        /* Show percentage text */
    bool indeterminate;          /* Indeterminate mode (animated) */
} ProgressBarComponent;

/* Initialize progress bar */
void ui_component_progress_init(ProgressBarComponent *bar, int y, int x, int w);

/* Set progress */
void ui_component_progress_set(ProgressBarComponent *bar, float percent, const char *label);

/* Draw progress bar */
void ui_component_progress_draw(const ProgressBarComponent *bar);

#endif /* UI_COMPONENTS_H */


#include "ui_theme.h"

#include <string.h>

void ui_theme_apply(struct nk_context *ctx) {
    if (!ctx) {
        return;
    }
    struct nk_color table[NK_COLOR_COUNT];
    memset(table, 0, sizeof(table));

    table[NK_COLOR_TEXT] = COLOR_LIGHT;
    table[NK_COLOR_WINDOW] = nk_rgba(25, 35, 45, 255);
    table[NK_COLOR_HEADER] = COLOR_SECONDARY;
    table[NK_COLOR_BORDER] = nk_rgba(60, 70, 80, 255);
    table[NK_COLOR_BUTTON] = COLOR_PRIMARY;
    table[NK_COLOR_BUTTON_HOVER] = nk_rgb(52, 152, 219);
    table[NK_COLOR_BUTTON_ACTIVE] = COLOR_GOLD;
    table[NK_COLOR_TOGGLE] = COLOR_PRIMARY;
    table[NK_COLOR_TOGGLE_HOVER] = COLOR_GOLD;
    table[NK_COLOR_TOGGLE_CURSOR] = COLOR_LIGHT;
    table[NK_COLOR_SELECT] = COLOR_PRIMARY;
    table[NK_COLOR_SELECT_ACTIVE] = COLOR_GOLD;
    table[NK_COLOR_SLIDER] = COLOR_PRIMARY;
    table[NK_COLOR_SLIDER_CURSOR] = COLOR_GOLD;
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = COLOR_LIGHT;
    table[NK_COLOR_PROPERTY] = COLOR_SECONDARY;
    table[NK_COLOR_EDIT] = COLOR_DARK;
    table[NK_COLOR_EDIT_CURSOR] = COLOR_LIGHT;
    table[NK_COLOR_COMBO] = COLOR_SECONDARY;
    table[NK_COLOR_CHART] = COLOR_PRIMARY;
    table[NK_COLOR_CHART_COLOR] = COLOR_GOLD;
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = COLOR_SUCCESS;
    table[NK_COLOR_SCROLLBAR] = COLOR_SECONDARY;
    table[NK_COLOR_SCROLLBAR_CURSOR] = COLOR_PRIMARY;
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = COLOR_GOLD;
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = COLOR_GOLD;
    table[NK_COLOR_TAB_HEADER] = COLOR_SECONDARY;

    nk_style_from_table(ctx, table);

    ctx->style.window.border_color = COLOR_GOLD;
    ctx->style.window.border = 2.0f;
    ctx->style.button.rounding = 6.0f;
    ctx->style.window.padding = nk_vec2(20, 20);
    ctx->style.window.background = nk_rgba(30, 40, 52, 245);
}

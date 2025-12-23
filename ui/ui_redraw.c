#include "ui_redraw.h"
#include <ncurses.h>
#include <string.h>

/* ============================================================================
 * OPTIMIZED REDRAW IMPLEMENTATION
 * ============================================================================
 * Tracks dirty regions and performs partial redraws to minimize flicker
 * and improve performance. Uses ncurses window buffering for smooth updates.
 * ============================================================================ */

void ui_redraw_init(DirtyFlags *flags)
{
    if (!flags)
        return;

    memset(flags, 0, sizeof(DirtyFlags));
    flags->last_state = UI_STATE_DASHBOARD;
    flags->last_term_rows = 0;
    flags->last_term_cols = 0;
}

void ui_redraw_mark_header(DirtyFlags *flags)
{
    if (flags)
        flags->header_dirty = true;
}

void ui_redraw_mark_sidebar(DirtyFlags *flags)
{
    if (flags)
        flags->sidebar_dirty = true;
}

void ui_redraw_mark_content(DirtyFlags *flags)
{
    if (flags)
        flags->content_dirty = true;
}

void ui_redraw_mark_footer(DirtyFlags *flags)
{
    if (flags)
        flags->footer_dirty = true;
}

void ui_redraw_mark_status(DirtyFlags *flags)
{
    if (flags)
        flags->status_dirty = true;
}

void ui_redraw_mark_dialog(DirtyFlags *flags)
{
    if (flags)
        flags->dialog_dirty = true;
}

void ui_redraw_mark_all(DirtyFlags *flags)
{
    if (!flags)
        return;
    flags->header_dirty = true;
    flags->sidebar_dirty = true;
    flags->content_dirty = true;
    flags->footer_dirty = true;
    flags->status_dirty = true;
    flags->dialog_dirty = true;
    flags->full_redraw = true;
}

bool ui_redraw_needed(const DirtyFlags *flags)
{
    if (!flags)
        return false;
    return flags->full_redraw ||
           flags->header_dirty ||
           flags->sidebar_dirty ||
           flags->content_dirty ||
           flags->footer_dirty ||
           flags->status_dirty ||
           flags->dialog_dirty;
}

bool ui_redraw_header_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->header_dirty);
}

bool ui_redraw_sidebar_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->sidebar_dirty);
}

bool ui_redraw_content_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->content_dirty);
}

bool ui_redraw_footer_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->footer_dirty);
}

bool ui_redraw_status_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->status_dirty);
}

bool ui_redraw_dialog_needed(const DirtyFlags *flags)
{
    return flags && (flags->full_redraw || flags->dialog_dirty);
}

void ui_redraw_clear(DirtyFlags *flags)
{
    if (!flags)
        return;
    flags->header_dirty = false;
    flags->sidebar_dirty = false;
    flags->content_dirty = false;
    flags->footer_dirty = false;
    flags->status_dirty = false;
    flags->dialog_dirty = false;
    flags->full_redraw = false;
}

void ui_redraw_clear_region(DirtyFlags *flags, int region)
{
    if (!flags)
        return;

    switch (region)
    {
    case 0:
        flags->header_dirty = false;
        break;
    case 1:
        flags->sidebar_dirty = false;
        break;
    case 2:
        flags->content_dirty = false;
        break;
    case 3:
        flags->footer_dirty = false;
        break;
    case 4:
        flags->status_dirty = false;
        break;
    case 5:
        flags->dialog_dirty = false;
        break;
    }
}

void ui_redraw_refresh_batch(void)
{
    /* Use doupdate() for batched refresh - reduces flicker */
    doupdate();
}

void ui_redraw_check_resize(DirtyFlags *flags, UIContext *ctx)
{
    if (!flags || !ctx)
        return;

    /* Check if terminal size changed */
    if (ctx->term_rows != flags->last_term_rows ||
        ctx->term_cols != flags->last_term_cols)
    {
        flags->last_term_rows = ctx->term_rows;
        flags->last_term_cols = ctx->term_cols;
        ui_redraw_mark_all(flags);
    }

    /* Check if state changed */
    if (ctx->current_state != flags->last_state)
    {
        flags->last_state = ctx->current_state;
        ui_redraw_mark_all(flags);
    }
}

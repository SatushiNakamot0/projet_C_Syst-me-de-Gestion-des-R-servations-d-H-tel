#ifndef UI_ROOMS_H
#define UI_ROOMS_H

#include "ui_state.h"
#include "../include/structures.h"

/**
 * Shows and handles the add room form as a self-contained modal
 * Returns 0 on success, -1 on cancel/error
 */
int show_add_room_form(Chambre chambres[], int *count, int capacity);

/**
 * Shows the edit room form
 * Returns 0 on success, -1 on cancel
 */
int show_edit_room_form(Chambre *room, Chambre chambres[], int count);

/**
 * Handles input for the rooms list state
 * Returns navigation direction
 */
NavDirection handle_rooms_input(UIContext *ctx, int key);

#endif /* UI_ROOMS_H */
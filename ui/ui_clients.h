#ifndef UI_CLIENTS_H
#define UI_CLIENTS_H

#include "../include/structures.h"
#include "ui_state.h"

/**
 * Handles input for the clients list state
 * Returns navigation direction
 */
int handle_clients_input(UIContext *ctx, int key);

/**
 * Shows the client search modal and returns selected client ID
 * Returns client ID on selection, -1 on cancel
 */
int show_client_search_modal(Client clients[], int clients_count);

/**
 * Shows the add client form (Safe Input Version)
 */
void show_add_client_form(Client *clients, int *count);

#endif /* UI_CLIENTS_H */
#ifndef UI_RESERVATIONS_H
#define UI_RESERVATIONS_H

#include "../include/structures.h"
#include "ui_state.h"

/**
 * Shows the 3-step reservation wizard modal
 * Returns 0 on success, -1 on cancel/error
 */
int show_reservation_wizard(Chambre chambres[], int chambres_count,
                            Reservation reservations[], int *reservations_count,
                            Client clients[], int clients_count);

/**
 * Shows the billing modal for a reservation
 */
void show_billing_modal(Reservation *reservation, Client *client, Chambre *room);

/**
 * Shows the edit reservation form
 * Returns 0 on success, -1 on cancel
 */
int show_edit_reservation_form(Reservation *res, Reservation reservations[], int count, Chambre chambres[], int chambres_count);

/**
 * Handles input for the reservations list state
 * Returns navigation direction (int cast)
 */
int handle_reservations_input(UIContext *ctx, int key);

#endif /* UI_RESERVATIONS_H */
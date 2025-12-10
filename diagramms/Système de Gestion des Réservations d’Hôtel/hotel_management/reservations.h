#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "structures.h"

bool reservations_initialiser(AppContext *app);
void reservations_liberer(AppContext *app);

bool reservations_creer(AppContext *app, Reservation *reservation, char *erreur, size_t erreur_taille);
bool reservations_modifier(AppContext *app, const Reservation *reservation, char *erreur, size_t erreur_taille);
bool reservations_annuler(AppContext *app, const char *reservation_id, char *erreur, size_t erreur_taille);

bool reservations_verifier_disponibilite(const AppContext *app, const char *chambre_numero, const Date *debut, const Date *fin, const char *reservation_id_a_ignorer);
uint32_t reservations_calculer_nuits(const Date *debut, const Date *fin);
double reservations_calculer_montant(const Chambre *chambre, uint32_t nb_nuits);

Reservation *reservations_trouver_par_id(const AppContext *app, const char *reservation_id);

#endif // RESERVATIONS_H

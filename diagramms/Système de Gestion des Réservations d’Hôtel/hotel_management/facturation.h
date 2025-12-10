#ifndef FACTURATION_H
#define FACTURATION_H

#include <stdbool.h>
#include <stddef.h>
#include "structures.h"

bool facturation_initialiser(AppContext *app);
void facturation_liberer(AppContext *app);

Facture *facturation_generer(AppContext *app, const Reservation *reservation, char *erreur, size_t erreur_taille);
Facture *facturation_trouver_par_id(const AppContext *app, const char *facture_id);

#endif // FACTURATION_H

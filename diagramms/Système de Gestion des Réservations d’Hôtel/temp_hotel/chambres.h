#ifndef CHAMBRES_H
#define CHAMBRES_H

#include <stdbool.h>
#include "structures.h"

bool chambres_initialiser(AppContext *app);
void chambres_liberer(AppContext *app);

bool chambres_ajouter(AppContext *app, const Chambre *chambre, char *erreur, size_t erreur_taille);
bool chambres_modifier(AppContext *app, const Chambre *chambre, char *erreur, size_t erreur_taille);
bool chambres_supprimer(AppContext *app, const char *numero, char *erreur, size_t erreur_taille);

Chambre *chambres_trouver_par_numero(const AppContext *app, const char *numero);
bool chambres_marquer_statut(AppContext *app, const char *numero, RoomStatus statut);

#endif // CHAMBRES_H

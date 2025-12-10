#ifndef CLIENTS_H
#define CLIENTS_H

#include <stdbool.h>
#include <stddef.h>
#include "structures.h"

bool clients_initialiser(AppContext *app);
void clients_liberer(AppContext *app);

bool clients_ajouter(AppContext *app, const Client *client, char *erreur, size_t erreur_taille);
bool clients_modifier(AppContext *app, const Client *client, char *erreur, size_t erreur_taille);
bool clients_supprimer(AppContext *app, const char *client_id, char *erreur, size_t erreur_taille);

Client *clients_trouver_par_id(const AppContext *app, const char *client_id);
size_t clients_rechercher_par_nom(const AppContext *app, const char *terme, Client **resultats);

#endif // CLIENTS_H

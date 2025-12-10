#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stdbool.h>
#include <stddef.h>
#include "structures.h"

bool authentication_initialiser(AppContext *app);
void authentication_liberer(AppContext *app);

bool authentication_creer_utilisateur(AppContext *app, const char *username, const char *password, UserRole role, bool actif, char *erreur, size_t erreur_taille);
bool authentication_login(AppContext *app, const char *username, const char *password, char *erreur, size_t erreur_taille);
void authentication_logout(AppContext *app);

const char *authentication_role_to_string(UserRole role);

#endif // AUTHENTICATION_H

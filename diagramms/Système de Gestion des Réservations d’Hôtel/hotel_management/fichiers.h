#ifndef FICHIERS_H
#define FICHIERS_H

#include <stddef.h>
#include <stdbool.h>
#include "structures.h"

#define DATA_DIR              "data"
#define CLIENTS_FILE          DATA_DIR "/clients.dat"
#define CHAMBRES_FILE         DATA_DIR "/chambres.dat"
#define RESERVATIONS_FILE     DATA_DIR "/reservations.dat"
#define FACTURES_FILE         DATA_DIR "/factures.dat"
#define USERS_FILE            DATA_DIR "/users.dat"

bool fichiers_initialiser(const char *data_dir);

bool fichiers_charger_clients(Client **clients, size_t *count);
bool fichiers_sauvegarder_clients(const Client *clients, size_t count);

bool fichiers_charger_chambres(Chambre **chambres, size_t *count);
bool fichiers_sauvegarder_chambres(const Chambre *chambres, size_t count);

bool fichiers_charger_reservations(Reservation **reservations, size_t *count);
bool fichiers_sauvegarder_reservations(const Reservation *reservations, size_t count);

bool fichiers_charger_factures(Facture **factures, size_t *count);
bool fichiers_sauvegarder_factures(const Facture *factures, size_t count);

bool fichiers_charger_utilisateurs(Utilisateur **utilisateurs, size_t *count);
bool fichiers_sauvegarder_utilisateurs(const Utilisateur *utilisateurs, size_t count);

#endif // FICHIERS_H

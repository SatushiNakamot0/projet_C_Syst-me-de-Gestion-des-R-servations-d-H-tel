
#ifndef CLIENTS_H
#define CLIENTS_H
#include "structures.h"

int ajouter_client(Client clients[], int *count);

void afficher_clients(const Client clients[], int count);

int rechercher_client_par_nom(const Client clients[], int count, const char *nom);

void modifier_client(Client clients[], int count);

void supprimer_client(Client clients[], int *count);

// **N'oubliez pas les sauvegardes !**
// Vous devrez appeler une fonction de sauvegarde après ajout/modification/suppression.
// Cette fonction est dans fichiers.h (ex: sauvegarder_clients(clients, *count))

#endif
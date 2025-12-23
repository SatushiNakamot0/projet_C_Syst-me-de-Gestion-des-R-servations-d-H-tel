#ifndef CLIENTS_H
#define CLIENTS_H
#include "structures.h"

void ajouter_client(Client clients[], int *count);

void afficher_clients(const Client clients[], int count);

void rechercher_client_par_nom(const Client clients[], int count);

void modifier_client(Client clients[], int count);

void supprimer_client(Client clients[], int *count);

#endif

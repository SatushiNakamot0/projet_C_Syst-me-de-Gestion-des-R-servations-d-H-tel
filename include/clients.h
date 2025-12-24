#ifndef CLIENTS_H
#define CLIENTS_H
#include "structures.h"

/* Add a new client - returns 0 on success, <0 on failure */
int client_ajouter(Client *clients, int *count, const Client *nouveau_client);

/* Modify an existing client - returns 0 on success, <0 on failure */
int client_modifier(Client *clients, int count, const Client *modifie);

/* Delete a client by ID - returns 0 on success, <0 on failure */
int client_supprimer(Client *clients, int *count, int id);

#endif

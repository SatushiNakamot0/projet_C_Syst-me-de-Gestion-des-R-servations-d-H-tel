#ifndef VALIDATION_H
#define VALIDATION_H

#include <stddef.h>
#include <stdbool.h>
#include "structures.h"

bool validation_email(const char *email);
bool validation_telephone(const char *telephone);
bool validation_date(const Date *date);
bool validation_date_range(const Date *start, const Date *end);
bool validation_client(const Client *client, char *erreur, size_t erreur_taille);
bool validation_chambre(const Chambre *chambre, char *erreur, size_t erreur_taille);
bool validation_reservation(const Reservation *reservation, char *erreur, size_t erreur_taille);

#endif // VALIDATION_H

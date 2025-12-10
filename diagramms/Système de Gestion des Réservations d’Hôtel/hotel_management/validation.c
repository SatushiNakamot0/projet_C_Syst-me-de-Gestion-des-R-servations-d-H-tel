#include "validation.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool is_leap_year(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

static int days_in_month(int month, int year) {
    static const int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    if (month < 1 || month > 12) {
        return 0;
    }
    return days[month - 1];
}

bool validation_email(const char *email) {
    if (!email) {
        return false;
    }
    const char *at = strchr(email, '@');
    if (!at || at == email) {
        return false;
    }
    const char *dot = strrchr(email, '.');
    if (!dot || dot < at) {
        return false;
    }
    if (*(dot + 1) == '\0') {
        return false;
    }
    return true;
}

bool validation_telephone(const char *telephone) {
    if (!telephone || *telephone == '\0') {
        return false;
    }
    size_t len = strlen(telephone);
    if (len < 6 || len > MAX_PHONE_LENGTH - 1) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        if (telephone[i] == ' ') {
            continue;
        }
        if (!(isdigit((unsigned char)telephone[i]) || telephone[i] == '+' || telephone[i] == '-')) {
            return false;
        }
    }
    return true;
}

bool validation_date(const Date *date) {
    if (!date) {
        return false;
    }
    if (date->annee < 1900 || date->annee > 2100) {
        return false;
    }
    int dim = days_in_month(date->mois, date->annee);
    if (dim == 0) {
        return false;
    }
    return date->jour >= 1 && date->jour <= dim;
}

bool validation_date_range(const Date *start, const Date *end) {
    if (!validation_date(start) || !validation_date(end)) {
        return false;
    }
    if (end->annee < start->annee) {
        return false;
    }
    if (end->annee == start->annee && end->mois < start->mois) {
        return false;
    }
    if (end->annee == start->annee && end->mois == start->mois && end->jour <= start->jour) {
        return false;
    }
    return true;
}

static void set_error(char *buffer, size_t buffer_len, const char *message) {
    if (buffer && buffer_len > 0 && message) {
        snprintf(buffer, buffer_len, "%s", message);
    }
}

bool validation_client(const Client *client, char *erreur, size_t erreur_taille) {
    if (!client) {
        set_error(erreur, erreur_taille, "Client invalide");
        return false;
    }
    if (client->nom[0] == '\0' || client->prenom[0] == '\0') {
        set_error(erreur, erreur_taille, "Nom et prénom requis");
        return false;
    }
    if (!validation_email(client->email)) {
        set_error(erreur, erreur_taille, "Email invalide");
        return false;
    }
    if (!validation_telephone(client->telephone)) {
        set_error(erreur, erreur_taille, "Téléphone invalide");
        return false;
    }
    return true;
}

bool validation_chambre(const Chambre *chambre, char *erreur, size_t erreur_taille) {
    if (!chambre) {
        set_error(erreur, erreur_taille, "Chambre invalide");
        return false;
    }
    if (chambre->numero[0] == '\0') {
        set_error(erreur, erreur_taille, "Numéro requis");
        return false;
    }
    if (chambre->prix_par_nuit <= 0.0) {
        set_error(erreur, erreur_taille, "Prix invalide");
        return false;
    }
    if (chambre->capacite == 0) {
        set_error(erreur, erreur_taille, "Capacité invalide");
        return false;
    }
    return true;
}

bool validation_reservation(const Reservation *reservation, char *erreur, size_t erreur_taille) {
    if (!reservation) {
        set_error(erreur, erreur_taille, "Réservation invalide");
        return false;
    }
    if (!validation_date_range(&reservation->check_in, &reservation->check_out)) {
        set_error(erreur, erreur_taille, "Dates invalides");
        return false;
    }
    if (reservation->montant_total < 0.0) {
        set_error(erreur, erreur_taille, "Montant négatif");
        return false;
    }
    return true;
}

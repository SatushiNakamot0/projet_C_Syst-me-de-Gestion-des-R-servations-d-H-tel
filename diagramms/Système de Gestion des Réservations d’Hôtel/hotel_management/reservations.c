#include "reservations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fichiers.h"
#include "validation.h"
#include "clients.h"
#include "chambres.h"

static int date_to_serial(const Date *date) {
    static const int days_before_month[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
    if (!date) {
        return 0;
    }
    int y = date->annee;
    int m = date->mois;
    int d = date->jour;
    int days = (y - 1900) * 365;
    days += (y - 1901) / 4 - (y - 1901) / 100 + (y - 1601) / 400; // approximate leap adjustments
    if (m > 1) {
        days += days_before_month[m - 1];
    }
    if (m > 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))) {
        days += 1;
    }
    days += d;
    return days;
}

static bool intervals_overlap(const Date *a_start, const Date *a_end, const Date *b_start, const Date *b_end) {
    int a_s = date_to_serial(a_start);
    int a_e = date_to_serial(a_end);
    int b_s = date_to_serial(b_start);
    int b_e = date_to_serial(b_end);
    return (a_s < b_e) && (b_s < a_e);
}

bool reservations_initialiser(AppContext *app) {
    if (!app) {
        return false;
    }
    free(app->repo.reservations);
    app->repo.reservations = NULL;
    app->repo.reservations_count = 0;
    if (!fichiers_charger_reservations(&app->repo.reservations, &app->repo.reservations_count)) {
        app->repo.reservations = NULL;
        app->repo.reservations_count = 0;
        return false;
    }
    return true;
}

void reservations_liberer(AppContext *app) {
    if (!app) {
        return;
    }
    free(app->repo.reservations);
    app->repo.reservations = NULL;
    app->repo.reservations_count = 0;
}

static bool persist_reservations(const AppContext *app) {
    return fichiers_sauvegarder_reservations(app->repo.reservations, app->repo.reservations_count);
}

Reservation *reservations_trouver_par_id(const AppContext *app, const char *reservation_id) {
    if (!app || !reservation_id) {
        return NULL;
    }
    for (size_t i = 0; i < app->repo.reservations_count; ++i) {
        if (strcmp(app->repo.reservations[i].id, reservation_id) == 0) {
            return &((Reservation *)app->repo.reservations)[i];
        }
    }
    return NULL;
}

uint32_t reservations_calculer_nuits(const Date *debut, const Date *fin) {
    if (!debut || !fin) {
        return 0;
    }
    int start = date_to_serial(debut);
    int end = date_to_serial(fin);
    if (end <= start) {
        return 0;
    }
    return (uint32_t)(end - start);
}

double reservations_calculer_montant(const Chambre *chambre, uint32_t nb_nuits) {
    if (!chambre) {
        return 0.0;
    }
    return chambre->prix_par_nuit * (double)nb_nuits;
}

bool reservations_verifier_disponibilite(const AppContext *app, const char *chambre_numero, const Date *debut, const Date *fin, const char *reservation_id_a_ignorer) {
    if (!app || !chambre_numero || !debut || !fin) {
        return false;
    }
    for (size_t i = 0; i < app->repo.reservations_count; ++i) {
        Reservation *res = &app->repo.reservations[i];
        if (reservation_id_a_ignorer && strcmp(res->id, reservation_id_a_ignorer) == 0) {
            continue;
        }
        if (strcmp(res->chambre_numero, chambre_numero) != 0) {
            continue;
        }
        if (res->statut == RESERVATION_ANNULEE) {
            continue;
        }
        if (intervals_overlap(debut, fin, &res->check_in, &res->check_out)) {
            return false;
        }
    }
    return true;
}

bool reservations_creer(AppContext *app, Reservation *reservation, char *erreur, size_t erreur_taille) {
    if (!app || !reservation) {
        return false;
    }
    if (!validation_reservation(reservation, erreur, erreur_taille)) {
        return false;
    }
    if (!clients_trouver_par_id(app, reservation->client_id)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Client introuvable");
        }
        return false;
    }
    Chambre *chambre = chambres_trouver_par_numero(app, reservation->chambre_numero);
    if (!chambre) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Chambre introuvable");
        }
        return false;
    }
    if (!reservations_verifier_disponibilite(app, chambre->numero, &reservation->check_in, &reservation->check_out, NULL)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Chambre indisponible");
        }
        return false;
    }
    reservation->nb_nuits = reservations_calculer_nuits(&reservation->check_in, &reservation->check_out);
    reservation->prix_nuit = chambre->prix_par_nuit;
    reservation->montant_total = reservations_calculer_montant(chambre, reservation->nb_nuits);
    reservation->statut = RESERVATION_CONFIRMEE;

    Reservation *liste = realloc(app->repo.reservations, sizeof(Reservation) * (app->repo.reservations_count + 1));
    if (!liste) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Mémoire insuffisante");
        }
        return false;
    }
    app->repo.reservations = liste;
    app->repo.reservations[app->repo.reservations_count] = *reservation;
    app->repo.reservations_count += 1;
    return persist_reservations(app);
}

bool reservations_modifier(AppContext *app, const Reservation *reservation, char *erreur, size_t erreur_taille) {
    if (!app || !reservation) {
        return false;
    }
    Reservation *existant = reservations_trouver_par_id(app, reservation->id);
    if (!existant) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Réservation introuvable");
        }
        return false;
    }
    if (!validation_reservation(reservation, erreur, erreur_taille)) {
        return false;
    }
    if (!reservations_verifier_disponibilite(app, reservation->chambre_numero, &reservation->check_in, &reservation->check_out, reservation->id)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Conflit de dates");
        }
        return false;
    }
    *existant = *reservation;
    return persist_reservations(app);
}

bool reservations_annuler(AppContext *app, const char *reservation_id, char *erreur, size_t erreur_taille) {
    if (!app || !reservation_id) {
        return false;
    }
    Reservation *res = reservations_trouver_par_id(app, reservation_id);
    if (!res) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Réservation introuvable");
        }
        return false;
    }
    res->statut = RESERVATION_ANNULEE;
    return persist_reservations(app);
}

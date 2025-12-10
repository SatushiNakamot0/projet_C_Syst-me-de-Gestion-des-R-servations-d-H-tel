#include "facturation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fichiers.h"

static bool persist_factures(const AppContext *app) {
    return fichiers_sauvegarder_factures(app->repo.factures, app->repo.factures_count);
}

bool facturation_initialiser(AppContext *app) {
    if (!app) {
        return false;
    }
    free(app->repo.factures);
    app->repo.factures = NULL;
    app->repo.factures_count = 0;
    if (!fichiers_charger_factures(&app->repo.factures, &app->repo.factures_count)) {
        app->repo.factures = NULL;
        app->repo.factures_count = 0;
        return false;
    }
    return true;
}

void facturation_liberer(AppContext *app) {
    if (!app) {
        return;
    }
    free(app->repo.factures);
    app->repo.factures = NULL;
    app->repo.factures_count = 0;
}

static Date date_du_jour(void) {
    time_t now = time(NULL);
    struct tm *info = localtime(&now);
    Date d = { 1,1,1900 };
    if (info) {
        d.jour = info->tm_mday;
        d.mois = info->tm_mon + 1;
        d.annee = info->tm_year + 1900;
    }
    return d;
}

Facture *facturation_trouver_par_id(const AppContext *app, const char *facture_id) {
    if (!app || !facture_id) {
        return NULL;
    }
    for (size_t i = 0; i < app->repo.factures_count; ++i) {
        if (strcmp(app->repo.factures[i].id, facture_id) == 0) {
            return &((Facture *)app->repo.factures)[i];
        }
    }
    return NULL;
}

Facture *facturation_generer(AppContext *app, const Reservation *reservation, char *erreur, size_t erreur_taille) {
    if (!app || !reservation) {
        return NULL;
    }
    Facture facture = {0};
    snprintf(facture.id, sizeof(facture.id), "FAC-%s", reservation->id);
    strncpy(facture.reservation_id, reservation->id, sizeof(facture.reservation_id) - 1);
    facture.date_emission = date_du_jour();
    facture.montant_total = reservation->montant_total;

    Facture *liste = realloc(app->repo.factures, sizeof(Facture) * (app->repo.factures_count + 1));
    if (!liste) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Mémoire insuffisante");
        }
        return NULL;
    }
    app->repo.factures = liste;
    app->repo.factures[app->repo.factures_count] = facture;
    Facture *result = &app->repo.factures[app->repo.factures_count];
    app->repo.factures_count += 1;

    if (!persist_factures(app)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Erreur de sauvegarde");
        }
        return NULL;
    }
    return result;
}

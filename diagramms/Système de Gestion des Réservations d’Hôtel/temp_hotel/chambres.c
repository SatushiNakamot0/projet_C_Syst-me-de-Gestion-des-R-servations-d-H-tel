#include "chambres.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "validation.h"
#include "fichiers.h"

bool chambres_initialiser(AppContext *app) {
    if (!app) {
        return false;
    }
    free(app->repo.chambres);
    app->repo.chambres = NULL;
    app->repo.chambres_count = 0;
    if (!fichiers_charger_chambres(&app->repo.chambres, &app->repo.chambres_count)) {
        app->repo.chambres = NULL;
        app->repo.chambres_count = 0;
        return false;
    }
    return true;
}

void chambres_liberer(AppContext *app) {
    if (!app) {
        return;
    }
    free(app->repo.chambres);
    app->repo.chambres = NULL;
    app->repo.chambres_count = 0;
}

Chambre *chambres_trouver_par_numero(const AppContext *app, const char *numero) {
    if (!app || !numero) {
        return NULL;
    }
    for (size_t i = 0; i < app->repo.chambres_count; ++i) {
        if (strcmp(app->repo.chambres[i].numero, numero) == 0) {
            return &((Chambre *)app->repo.chambres)[i];
        }
    }
    return NULL;
}

static bool persist_chambres(const AppContext *app) {
    return fichiers_sauvegarder_chambres(app->repo.chambres, app->repo.chambres_count);
}

bool chambres_ajouter(AppContext *app, const Chambre *chambre, char *erreur, size_t erreur_taille) {
    if (!app || !chambre) {
        return false;
    }
    if (!validation_chambre(chambre, erreur, erreur_taille)) {
        return false;
    }
    if (chambres_trouver_par_numero(app, chambre->numero)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Chambre existante");
        }
        return false;
    }
    Chambre *liste = realloc(app->repo.chambres, sizeof(Chambre) * (app->repo.chambres_count + 1));
    if (!liste) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "M�moire insuffisante");
        }
        return false;
    }
    app->repo.chambres = liste;
    app->repo.chambres[app->repo.chambres_count] = *chambre;
    app->repo.chambres_count += 1;
    return persist_chambres(app);
}

bool chambres_modifier(AppContext *app, const Chambre *chambre, char *erreur, size_t erreur_taille) {
    if (!app || !chambre) {
        return false;
    }
    Chambre *existant = chambres_trouver_par_numero(app, chambre->numero);
    if (!existant) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Chambre introuvable");
        }
        return false;
    }
    if (!validation_chambre(chambre, erreur, erreur_taille)) {
        return false;
    }
    *existant = *chambre;
    return persist_chambres(app);
}

bool chambres_supprimer(AppContext *app, const char *numero, char *erreur, size_t erreur_taille) {
    if (!app || !numero) {
        return false;
    }
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < app->repo.chambres_count; ++i) {
        if (strcmp(app->repo.chambres[i].numero, numero) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Chambre introuvable");
        }
        return false;
    }
    for (size_t i = index; i + 1 < app->repo.chambres_count; ++i) {
        app->repo.chambres[i] = app->repo.chambres[i + 1];
    }
    app->repo.chambres_count -= 1;
    if (app->repo.chambres_count == 0) {
        free(app->repo.chambres);
        app->repo.chambres = NULL;
    } else {
        Chambre *tmp = realloc(app->repo.chambres, sizeof(Chambre) * app->repo.chambres_count);
        if (tmp) {
            app->repo.chambres = tmp;
        }
    }
    return persist_chambres(app);
}

bool chambres_marquer_statut(AppContext *app, const char *numero, RoomStatus statut) {
    Chambre *chambre = chambres_trouver_par_numero(app, numero);
    if (!chambre) {
        return false;
    }
    chambre->statut = statut;
    return persist_chambres(app);
}

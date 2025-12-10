#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "fichiers.h"
#include "clients.h"
#include "chambres.h"
#include "reservations.h"
#include "facturation.h"
#include "authentication.h"
#include "menu.h"

static void liberer_modules(AppContext *app) {
    facturation_liberer(app);
    reservations_liberer(app);
    chambres_liberer(app);
    clients_liberer(app);
    authentication_liberer(app);
}

int main(void) {
    AppContext app;
    memset(&app, 0, sizeof(AppContext));

    if (!fichiers_initialiser(DATA_DIR)) {
        fprintf(stderr, "Impossible d'initialiser le dossier de données.\n");
        return EXIT_FAILURE;
    }

    if (!clients_initialiser(&app)) {
        fprintf(stderr, "Erreur chargement clients.\n");
    }
    if (!chambres_initialiser(&app)) {
        fprintf(stderr, "Erreur chargement chambres.\n");
    }
    if (!reservations_initialiser(&app)) {
        fprintf(stderr, "Erreur chargement réservations.\n");
    }
    if (!facturation_initialiser(&app)) {
        fprintf(stderr, "Erreur chargement factures.\n");
    }
    if (!authentication_initialiser(&app)) {
        fprintf(stderr, "Erreur chargement utilisateurs.\n");
    }

    if (!menu_initialize(&app)) {
        fprintf(stderr, "Initialisation UI échouée.\n");
        liberer_modules(&app);
        return EXIT_FAILURE;
    }

    menu_run(&app);

    menu_shutdown(&app);
    liberer_modules(&app);

    return EXIT_SUCCESS;
}

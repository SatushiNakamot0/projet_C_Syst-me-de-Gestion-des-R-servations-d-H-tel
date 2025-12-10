#include "clients.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "validation.h"
#include "fichiers.h"

bool clients_initialiser(AppContext *app) {
    if (!app) {
        return false;
    }
    free(app->repo.clients);
    app->repo.clients = NULL;
    app->repo.clients_count = 0;

    if (!fichiers_charger_clients(&app->repo.clients, &app->repo.clients_count)) {
        app->repo.clients = NULL;
        app->repo.clients_count = 0;
        return false;
    }
    return true;
}

void clients_liberer(AppContext *app) {
    if (!app) {
        return;
    }
    free(app->repo.clients);
    app->repo.clients = NULL;
    app->repo.clients_count = 0;
}

Client *clients_trouver_par_id(const AppContext *app, const char *client_id) {
    if (!app || !client_id) {
        return NULL;
    }
    for (size_t i = 0; i < app->repo.clients_count; ++i) {
        if (strcmp(app->repo.clients[i].id, client_id) == 0) {
            return &((Client *)app->repo.clients)[i];
        }
    }
    return NULL;
}

static bool persist_clients(const AppContext *app) {
    return fichiers_sauvegarder_clients(app->repo.clients, app->repo.clients_count);
}

bool clients_ajouter(AppContext *app, const Client *client, char *erreur, size_t erreur_taille) {
    if (!app || !client) {
        return false;
    }
    if (!validation_client(client, erreur, erreur_taille)) {
        return false;
    }
    if (clients_trouver_par_id(app, client->id)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "ID client d�j� utilis�");
        }
        return false;
    }

    Client *nouvelle_liste = realloc(app->repo.clients, sizeof(Client) * (app->repo.clients_count + 1));
    if (!nouvelle_liste) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "M�moire insuffisante");
        }
        return false;
    }

    app->repo.clients = nouvelle_liste;
    app->repo.clients[app->repo.clients_count] = *client;
    app->repo.clients_count += 1;

    if (!persist_clients(app)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Erreur de sauvegarde");
        }
        return false;
    }
    return true;
}

bool clients_modifier(AppContext *app, const Client *client, char *erreur, size_t erreur_taille) {
    if (!app || !client) {
        return false;
    }
    Client *existant = clients_trouver_par_id(app, client->id);
    if (!existant) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Client introuvable");
        }
        return false;
    }
    if (!validation_client(client, erreur, erreur_taille)) {
        return false;
    }
    *existant = *client;
    if (!persist_clients(app)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Erreur de sauvegarde");
        }
        return false;
    }
    return true;
}

bool clients_supprimer(AppContext *app, const char *client_id, char *erreur, size_t erreur_taille) {
    if (!app || !client_id) {
        return false;
    }
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < app->repo.clients_count; ++i) {
        if (strcmp(app->repo.clients[i].id, client_id) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Client introuvable");
        }
        return false;
    }
    for (size_t i = index; i + 1 < app->repo.clients_count; ++i) {
        app->repo.clients[i] = app->repo.clients[i + 1];
    }
    app->repo.clients_count -= 1;
    if (app->repo.clients_count == 0) {
        free(app->repo.clients);
        app->repo.clients = NULL;
    } else {
        Client *tmp = realloc(app->repo.clients, sizeof(Client) * app->repo.clients_count);
        if (tmp) {
            app->repo.clients = tmp;
        }
    }
    if (!persist_clients(app)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Erreur de sauvegarde");
        }
        return false;
    }
    return true;
}

size_t clients_rechercher_par_nom(const AppContext *app, const char *terme, Client **resultats) {
    if (!app || !terme || !resultats) {
        return 0;
    }
    *resultats = NULL;
    if (app->repo.clients_count == 0) {
        return 0;
    }
    Client *tmp = malloc(sizeof(Client) * app->repo.clients_count);
    if (!tmp) {
        return 0;
    }
    size_t count = 0;
    for (size_t i = 0; i < app->repo.clients_count; ++i) {
        if (strstr(app->repo.clients[i].nom, terme) || strstr(app->repo.clients[i].prenom, terme)) {
            tmp[count++] = app->repo.clients[i];
        }
    }
    if (count == 0) {
        free(tmp);
        return 0;
    }
    Client *res = realloc(tmp, sizeof(Client) * count);
    if (!res) {
        res = tmp;
    }
    *resultats = res;
    return count;
}

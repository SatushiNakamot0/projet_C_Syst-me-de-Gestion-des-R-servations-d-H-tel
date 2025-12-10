#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nuklear_config.h"
#include "ui_theme.h"
#include "authentication.h"
#include "notifications.h"

static bool afficher_login_console(AppContext *app) {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("\n===== Connexion =====\n");
    printf("Utilisateur: ");
    if (!fgets(username, sizeof(username), stdin)) {
        return false;
    }
    username[strcspn(username, "\n")] = '\0';

    printf("Mot de passe: ");
    if (!fgets(password, sizeof(password), stdin)) {
        return false;
    }
    password[strcspn(password, "\n")] = '\0';

    char erreur[128] = {0};
    if (!authentication_login(app, username, password, erreur, sizeof(erreur))) {
        printf("Erreur: %s\n", erreur);
        notifications_push(&app->notifications, NOTIFICATION_ERROR, erreur, 3.0);
        return false;
    }
    notifications_push(&app->notifications, NOTIFICATION_SUCCESS, "Connexion r�ussie", 3.0);
    return true;
}

static void afficher_dashboard_console(const AppContext *app) {
    printf("\n===== Tableau de bord (mode console temporaire) =====\n");
    printf("Clients: %zu | Chambres: %zu | R�servations: %zu | Factures: %zu\n",
           app->repo.clients_count,
           app->repo.chambres_count,
           app->repo.reservations_count,
           app->repo.factures_count);
    if (app->session.is_authenticated) {
        printf("Connect� en tant que %s (%s)\n",
               app->session.utilisateur.username,
               authentication_role_to_string(app->session.utilisateur.role));
    }
}

bool menu_initialize(AppContext *app) {
    if (!app) {
        return false;
    }
    notifications_init(&app->notifications);

    app->ui.ctx = malloc(sizeof(struct nk_context));
    if (!app->ui.ctx) {
        return false;
    }
    if (!nk_init_default(app->ui.ctx, 0)) {
        free(app->ui.ctx);
        app->ui.ctx = NULL;
        return false;
    }
    ui_theme_apply(app->ui.ctx);
    app->ui.palette = NULL;
    return true;
}

void menu_shutdown(AppContext *app) {
    if (!app) {
        return;
    }
    if (app->ui.ctx) {
        nk_free(app->ui.ctx);
        free(app->ui.ctx);
        app->ui.ctx = NULL;
    }
}

bool menu_run(AppContext *app) {
    if (!app) {
        return false;
    }
    printf("\n*** Syst�me de Gestion d'H�tel - Mode prototype console ***\n");
    printf("(L'interface Nuklear compl�te sera branch�e sur un backend graphique prochainement.)\n");

    if (!app->session.is_authenticated) {
        if (!afficher_login_console(app)) {
            return false;
        }
    }

    afficher_dashboard_console(app);
    printf("Tapez 'q' pour quitter, autre pour d�connexion: ");
    char buffer[8];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        if (buffer[0] == 'q' || buffer[0] == 'Q') {
            return true;
        }
    }
    authentication_logout(app);
    return true;
}

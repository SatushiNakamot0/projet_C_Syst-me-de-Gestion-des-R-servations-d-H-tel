#include "authentication.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sha256.h"
#include "fichiers.h"

static bool rng_seeded = false;

static void ensure_rng_seeded(void) {
    if (!rng_seeded) {
        srand((unsigned int)time(NULL));
        rng_seeded = true;
    }
}

static void generer_salt(uint8_t salt[16]) {
    ensure_rng_seeded();
    for (size_t i = 0; i < 16; ++i) {
        salt[i] = (uint8_t)(rand() % 256);
    }
}

static void calculer_hash(const char *password, const uint8_t salt[16], uint8_t hash[32]) {
    uint8_t buffer[16 + MAX_PASSWORD_LENGTH];
    size_t pass_len = strlen(password);
    if (pass_len > MAX_PASSWORD_LENGTH) {
        pass_len = MAX_PASSWORD_LENGTH;
    }
    memcpy(buffer, salt, 16);
    memcpy(buffer + 16, password, pass_len);
    sha256_hash(buffer, 16 + pass_len, hash);
}

static bool persist_utilisateurs(const AppContext *app) {
    return fichiers_sauvegarder_utilisateurs(app->repo.utilisateurs, app->repo.utilisateurs_count);
}

static Utilisateur *trouver_utilisateur(AppContext *app, const char *username) {
    if (!app || !username) {
        return NULL;
    }
    for (size_t i = 0; i < app->repo.utilisateurs_count; ++i) {
        if (strcmp(app->repo.utilisateurs[i].username, username) == 0) {
            return &app->repo.utilisateurs[i];
        }
    }
    return NULL;
}

static bool creer_compte_par_defaut(AppContext *app) {
    Utilisateur admin = {0};
    snprintf(admin.username, sizeof(admin.username), "admin");
    admin.role = ROLE_ADMIN;
    admin.actif = true;
    generer_salt(admin.salt);
    calculer_hash("admin", admin.salt, admin.password_hash);

    Utilisateur *liste = realloc(app->repo.utilisateurs, sizeof(Utilisateur));
    if (!liste) {
        return false;
    }
    app->repo.utilisateurs = liste;
    app->repo.utilisateurs[0] = admin;
    app->repo.utilisateurs_count = 1;
    return persist_utilisateurs(app);
}

bool authentication_initialiser(AppContext *app) {
    if (!app) {
        return false;
    }
    free(app->repo.utilisateurs);
    app->repo.utilisateurs = NULL;
    app->repo.utilisateurs_count = 0;
    if (!fichiers_charger_utilisateurs(&app->repo.utilisateurs, &app->repo.utilisateurs_count)) {
        app->repo.utilisateurs = NULL;
        app->repo.utilisateurs_count = 0;
        return false;
    }
    if (app->repo.utilisateurs_count == 0) {
        return creer_compte_par_defaut(app);
    }
    return true;
}

void authentication_liberer(AppContext *app) {
    if (!app) {
        return;
    }
    free(app->repo.utilisateurs);
    app->repo.utilisateurs = NULL;
    app->repo.utilisateurs_count = 0;
}

bool authentication_creer_utilisateur(AppContext *app, const char *username, const char *password, UserRole role, bool actif, char *erreur, size_t erreur_taille) {
    if (!app || !username || !password) {
        return false;
    }
    if (strlen(username) == 0 || strlen(password) < 6) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Identifiants invalides");
        }
        return false;
    }
    if (trouver_utilisateur(app, username)) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Utilisateur existant");
        }
        return false;
    }
    Utilisateur *liste = realloc(app->repo.utilisateurs, sizeof(Utilisateur) * (app->repo.utilisateurs_count + 1));
    if (!liste) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Mémoire insuffisante");
        }
        return false;
    }
    app->repo.utilisateurs = liste;
    Utilisateur *user = &app->repo.utilisateurs[app->repo.utilisateurs_count];
    memset(user, 0, sizeof(Utilisateur));
    strncpy(user->username, username, sizeof(user->username) - 1);
    user->role = role;
    user->actif = actif;
    generer_salt(user->salt);
    calculer_hash(password, user->salt, user->password_hash);
    app->repo.utilisateurs_count += 1;
    return persist_utilisateurs(app);
}

bool authentication_login(AppContext *app, const char *username, const char *password, char *erreur, size_t erreur_taille) {
    if (!app || !username || !password) {
        return false;
    }
    Utilisateur *user = trouver_utilisateur(app, username);
    if (!user || !user->actif) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Utilisateur invalide");
        }
        return false;
    }
    uint8_t hash[32];
    calculer_hash(password, user->salt, hash);
    if (memcmp(hash, user->password_hash, sizeof(hash)) != 0) {
        if (erreur) {
            snprintf(erreur, erreur_taille, "Mot de passe incorrect");
        }
        return false;
    }
    app->session.is_authenticated = true;
    app->session.utilisateur = *user;
    return true;
}

void authentication_logout(AppContext *app) {
    if (!app) {
        return;
    }
    app->session.is_authenticated = false;
    memset(&app->session.utilisateur, 0, sizeof(app->session.utilisateur));
}

const char *authentication_role_to_string(UserRole role) {
    switch (role) {
        case ROLE_ADMIN: return "Administrateur";
        case ROLE_RECEPTIONIST: return "Réceptionniste";
        case ROLE_CLIENT: return "Client";
        default: return "Inconnu";
    }
}

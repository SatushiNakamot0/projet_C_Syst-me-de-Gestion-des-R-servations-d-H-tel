# Architecture du Système de Gestion d''Hôtel

## Vue d''ensemble
- **Langage** : C (C11)
- **UI** : Nuklear + backend SDL2/OpenGL (intégration prévue dans `menu.c`)
- **Build** : CMake
- **Persistance** : fichiers binaires `.dat` dans `data/`
- **Modules** : clients, chambres, réservations, facturation, authentification, validation, notifications, thème UI, drivers fichiers.

## Modules
1. `structures.h`
   - Définit toutes les constantes, énumérations, structures (clients, chambres, réservations, factures, utilisateurs, dates, sessions, notifications, contexte app).
2. `validation.c/h`
   - Vérifie formats (email, téléphone), cohérence des dates `JJ/MM/AAAA`, disponibilité chambres, montants.
3. `fichiers.c/h`
   - Accès centralisé aux fichiers binaires, fonction utilitaires (lecture/écriture sécurisée, verrous, initialisation).
4. `clients.c/h`, `chambres.c/h`, `reservations.c/h`, `facturation.c/h`
   - CRUD, recherches, calculs spécifiques.
5. `authentication.c/h`
   - Gestion des comptes multiples, hash SHA-256 avec sel, session utilisateur, autorisations selon rôle.
6. `notifications.c/h`
   - File de toasts (succès/erreur/info) consommée par l''UI.
7. `ui_theme.c/h`
   - Palette "Hotel Luxury" + helpers pour appliquer le thème à Nuklear.
8. `menu.c/h`
   - Boucle principale, navigation multi-écrans (login, dashboard, clients, chambres, réservations, factures, calendrier, paramètres).

## Flux principal
1. `main.c` initialise `AppContext`, charge les données en mémoire (buffers dynamiques), configure Nuklear + thème.
2. L''écran de connexion (`menu_show_login`) authentifie l''utilisateur via `authentication_login`.
3. Après connexion, le `Dashboard` consomme les données agrégées (clients actifs, taux d''occupation, revenus) et gère navigation via menu latéral.
4. Chaque écran CRUD manipule les modules métier (ex : `clients_add`, `reservations_check_availability`) puis persiste via `fichiers_*`.
5. Génération factures déclenchée après confirmation de réservation ou via module facturation.
6. Notifications et modales sont gérées par `notifications.c` et rendues dans `menu.c`.

## Données & Validation
- **Clients** : ID unique, champs obligatoires, validation email/téléphone.
- **Chambres** : numéro unique, type, prix/nuit, statut (`DISPONIBLE`, `OCCUPEE`, `MAINTENANCE`).
- **Réservations** : check disponibilité avant insertion, calcul nuits/total, statut (`CONFIRMEE`, `ANNULEE`, `EN_ATTENTE`).
- **Factures** : montant = nuits  prix/nuit, date émission auto.
- **Authentification** : stockage `username`, `role`, `salt[16]`, `hash[32]`. Hash = SHA-256(salt || mot_de_passe).

## UI Nuklear
- Palette fournie appliquée via `nk_style_from_table`.
- Composants :
  - Sidebar icônes (Dash, Clients, Chambres, Réservations, Factures, Paramètres, Quitter).
  - Header (logo, rôle utilisateur, bouton déconnexion).
  - Cartes KPI, barres de progression, tableaux, formulaire modal.
  - Notifications toast (coin supérieur droit), modales de confirmation.
  - Calendrier (vue grille par chambre/jour) avec légende.

## Prochaines étapes
1. Implémenter `structures.h` et squelettes `.c/.h`.
2. Ajouter implémentation SHA-256 et module authentification minimal.
3. Mettre en place backend SDL2 + Nuklear (initialisation, boucle).
4. Implémenter CRUD clients/chambres + persistance.
5. Élargir UI (réservations, facturation, calendrier, notifications).

# Système de Gestion des Réservations d'Hôtel

Ce projet est une application C (C11) avec interface graphique **Nuklear** visant à gérer les clients, chambres, réservations et facturation d'un hôtel. Il intègre un système d'authentification à rôles multiples avec mots de passe hashés, une validation avancée, ainsi qu'un tableau de bord moderne inspiré du thème "Hotel Luxury".

## Fonctionnalités prévues
- Authentification multi-rôles (Administrateur, Réceptionniste, Client)
- Gestion des clients, chambres, réservations, factures
- Vérifications de disponibilité et calcul automatique des montants
- Notifications toast, modales de confirmation, calendrier des réservations
- Persistance dans des fichiers binaires (.dat)

## Pré-requis
- CMake >= 3.16
- Compilateur compatible C11
- Backend pour Nuklear (SDL2 + OpenGL recommandé). Les hooks seront ajoutés dans `menu.c`.

## Construction
```
cmake -S . -B build
cmake --build build
```

## Organisation
Consultez `docs/architecture.md` pour les détails sur l'architecture et la feuille de route.

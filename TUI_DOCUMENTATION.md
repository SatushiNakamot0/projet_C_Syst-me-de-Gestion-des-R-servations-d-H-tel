# Terminal User Interface (TUI) - Documentation Complète

## 🎨 Philosophie de Design

Cette interface TUI a été conçue selon les principes suivants :

1. **Clarté Visuelle** : Utilisation de caractères de dessin de boîtes (box-drawing) pour des bordures nettes
2. **Navigation Fluide** : Navigation entièrement au clavier, sans souris nécessaire
3. **Esthétique Professionnelle** : Palette de couleurs sombre avec accents colorés pour les messages
4. **Zéro Clutter** : Interface épurée, chaque élément a un but précis
5. **Responsive** : S'adapte automatiquement à la taille du terminal

## 🏗️ Architecture

### Structure Modulaire

```
ui/
├── ui.h / ui.c              # Point d'entrée principal, machine à états
├── ui_state.h               # Définitions des états et contexte
├── ui_theme.h / ui_theme.c # Système de couleurs et thème
├── ui_layout.h / ui_layout.c # Moteur de layout responsive
├── ui_draw.h / ui_draw.c   # Fonctions de dessin pour chaque écran
├── ui_input.h / ui_input.c # Gestion des entrées clavier
└── ui_utils.h / ui_utils.c # Fonctions utilitaires
```

### Machine à États

Le système utilise une machine à états finis pour gérer la navigation :

```
Dashboard → Clients → Rooms → Reservations → Billing → Help
    ↑         ↓         ↓          ↓           ↓        ↓
    └─────────┴─────────┴──────────┴───────────┴────────┘
                    (Navigation via sidebar)
```

**États disponibles :**
- `UI_STATE_DASHBOARD` - Écran d'accueil avec KPIs
- `UI_STATE_CLIENTS` - Gestion des clients
- `UI_STATE_ROOMS` - Gestion des chambres
- `UI_STATE_RESERVATIONS` - Gestion des réservations
- `UI_STATE_BILLING` - Gestion de la facturation
- `UI_STATE_HELP` - Aide et raccourcis clavier

### Composants UI

#### 1. Header
- Titre de l'application
- Date et heure actuelles
- Fond bleu foncé avec texte blanc

#### 2. Sidebar
- Menu de navigation principal
- Item sélectionné surligné en bleu
- Navigation avec flèches haut/bas

#### 3. Content Area
- Zone dynamique qui change selon l'état
- Tables avec en-têtes colorés
- Barres de progression pour les KPIs
- Formulaires pour ajout/édition

#### 4. Footer
- Raccourcis clavier disponibles
- Messages de statut temporaires
- Fond gris foncé

## 🎨 Palette de Couleurs

| Couleur | Usage | Code |
|---------|-------|------|
| Blanc | Texte par défaut | `COLOR_PAIR_DEFAULT` |
| Cyan | Titres | `COLOR_PAIR_TITLE` |
| Bleu | En-têtes, sélection | `COLOR_PAIR_HEADER` |
| Vert | Messages de succès | `COLOR_PAIR_SUCCESS` |
| Jaune | Avertissements | `COLOR_PAIR_WARNING` |
| Rouge | Erreurs | `COLOR_PAIR_ERROR` |
| Gris | Texte secondaire | `COLOR_PAIR_DIM` |

## ⌨️ Navigation et Raccourcis

### Navigation Principale

| Touche | Action |
|--------|--------|
| `↑` `↓` | Naviguer dans les menus/listes |
| `←` `→` | Navigation horizontale (si applicable) |
| `Enter` | Sélectionner / Confirmer |
| `ESC` | Retour / Annuler |
| `Q` | Quitter l'application |
| `F1` | Afficher l'aide |

### Raccourcis Rapides

| Touche | Destination |
|--------|-------------|
| `1` | Dashboard |
| `2` | Clients |
| `3` | Rooms |
| `4` | Reservations |
| `5` | Billing |

### Actions dans les Listes

| Touche | Action |
|--------|--------|
| `A` | Ajouter un nouvel élément |
| `E` | Éditer l'élément sélectionné |
| `D` | Supprimer l'élément sélectionné |
| `S` | Rechercher |

## 📐 Layout Responsive

Le système de layout s'adapte automatiquement à la taille du terminal :

- **Taille minimale** : 80x24 caractères
- **Sidebar** : 20 caractères de largeur (18 sur petits terminaux)
- **Content** : Reste de l'espace disponible
- **Header/Footer** : 1 ligne chacun

### Gestion du Redimensionnement

Le système écoute le signal `SIGWINCH` et redessine automatiquement l'interface lors du redimensionnement du terminal.

## 🖥️ Écrans Disponibles

### 1. Dashboard

Affiche :
- **Total Clients** : Nombre de clients enregistrés
- **Total Rooms** : Nombre de chambres disponibles
- **Occupancy Rate** : Taux d'occupation avec barre de progression
- **Reservations** : Nombre de réservations actives
- **Total Revenue** : Revenus totaux en EUR

### 2. Clients Management

**Liste des clients** :
- Table avec colonnes : ID, Nom, Prénom, Email, Téléphone
- Navigation avec flèches haut/bas
- Actions : Add, Edit, Delete, Search

**Ajout/Édition** :
- Formulaire avec validation en temps réel
- Vérification d'email unique
- Sauvegarde automatique

### 3. Rooms Management

- Liste des chambres avec statut
- Code couleur pour disponibilité
- Gestion des types de chambres
- Prix par nuit

### 4. Reservations

- Liste des réservations actives
- Visualisation calendrier (à implémenter)
- Vérification de disponibilité
- Calcul automatique du montant

### 5. Billing

- Liste des factures
- Détails : Client, Nuits, Prix/Nuit, Total
- Création de nouvelles factures
- Export (à implémenter)

### 6. Help

- Liste complète des raccourcis clavier
- Guide de navigation
- Instructions pour chaque écran

## 🔧 Compilation

### Prérequis

- **GCC** (GNU Compiler Collection)
- **ncurses** (bibliothèque pour TUI)
- **Make** (système de build)

### Installation de ncurses

**Ubuntu/Debian :**
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

**Fedora/RHEL :**
```bash
sudo dnf install ncurses-devel
```

**macOS :**
```bash
brew install ncurses
```

### Compilation

```bash
make clean
make
```

Cela génère l'exécutable `hotel_app`.

### Options de Compilation

Le Makefile utilise :
- `-Wall -Wextra -pedantic` : Avertissements stricts
- `-std=c99` : Standard C99
- `-lncurses` : Lien avec ncurses

## 🚀 Exécution

```bash
./hotel_app
```

**Note importante** : L'application nécessite un terminal qui supporte :
- Couleurs (256 couleurs recommandé)
- Caractères de dessin de boîtes (box-drawing)
- UTF-8 (optionnel, pour icônes)

### Terminal Recommandés

- **Linux** : gnome-terminal, konsole, xterm
- **macOS** : Terminal.app, iTerm2
- **Windows** : Windows Terminal (avec WSL), Git Bash

## 📊 Structure des Données

Les données sont sauvegardées dans le dossier `data/` :

```
data/
├── clients.dat      # Fichier binaire des clients
├── chambres.dat     # Fichier binaire des chambres
├── reservations.dat # Fichier binaire des réservations
└── factures.dat     # Fichier binaire des factures
```

Format : Binaire avec en-tête de comptage pour efficacité.

## 🧪 Tests et Validation

### Checklist de Test

- [ ] Compilation sans erreurs ni avertissements
- [ ] Affichage correct du dashboard
- [ ] Navigation dans tous les menus
- [ ] Ajout/édition/suppression de clients
- [ ] Affichage correct des tables
- [ ] Messages de statut fonctionnels
- [ ] Redimensionnement du terminal
- [ ] Sauvegarde/chargement des données
- [ ] Gestion des erreurs (fichiers manquants, etc.)

### Tests de Performance

- **Temps de démarrage** : < 100ms
- **Temps de redessin** : < 16ms (60 FPS)
- **Mémoire** : < 10MB pour 1000 clients

## 🐛 Dépannage

### Problème : Couleurs non affichées

**Solution** : Vérifiez que votre terminal supporte les couleurs :
```bash
echo $TERM
# Doit afficher quelque chose comme xterm-256color
```

### Problème : Caractères de boîtes incorrects

**Solution** : Configurez votre terminal pour UTF-8 :
```bash
export LANG=en_US.UTF-8
```

### Problème : ncurses non trouvé

**Solution** : Installez les headers de développement :
```bash
sudo apt-get install libncurses-dev  # Ubuntu/Debian
```

## 🔮 Améliorations Futures

1. **Formulaires interactifs** : Saisie complète avec validation
2. **Recherche avancée** : Filtres multiples, recherche fuzzy
3. **Calendrier visuel** : Vue calendrier pour les réservations
4. **Export PDF** : Génération de factures en PDF
5. **Thèmes** : Plusieurs thèmes (clair, sombre, haute contraste)
6. **Raccourcis personnalisables** : Configuration des touches
7. **Historique** : Undo/Redo pour les actions
8. **Notifications toast** : Système de notifications amélioré

## 📝 Notes Techniques

### Gestion Mémoire

- Allocation dynamique pour les tableaux de données
- Libération automatique à la fermeture
- Pas de fuites mémoire (vérifié avec Valgrind)

### Thread Safety

- Application mono-thread
- Pas de problèmes de concurrence
- Signal handlers sécurisés

### Portabilité

- Compatible POSIX
- Testé sur Linux et macOS
- Windows nécessite WSL ou Cygwin

## 📚 Références

- [ncurses Programming HOWTO](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [C99 Standard](https://en.cppreference.com/w/c/99)
- [ANSI Escape Codes](https://en.wikipedia.org/wiki/ANSI_escape_code)

## 👥 Contribution

Pour contribuer au projet :

1. Respecter le style de code (C99, indentation 4 espaces)
2. Ajouter des commentaires pour les fonctions complexes
3. Tester sur plusieurs terminaux
4. Vérifier avec `-Wall -Wextra -pedantic`

---

**Version** : 1.0.0  
**Dernière mise à jour** : 2025  
**Auteur** : Système de Gestion d'Hôtel - Équipe de Développement


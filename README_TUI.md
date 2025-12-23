# 🏨 Système de Gestion d'Hôtel - Interface TUI

## Vue d'Ensemble

Interface terminal professionnelle (TUI) pour le système de gestion d'hôtel, développée en **C pur** avec **ncurses**.

## ✨ Caractéristiques

- 🎨 **Interface moderne** avec caractères de dessin de boîtes
- ⌨️ **Navigation entièrement au clavier**
- 🎨 **Palette de couleurs professionnelle**
- 📱 **Layout responsive** qui s'adapte au terminal
- 🔄 **Machine à états** pour navigation fluide
- 💾 **Persistance automatique** des données

## 🚀 Démarrage Rapide

### Installation

```bash
# Installer ncurses (Ubuntu/Debian)
sudo apt-get install libncurses5-dev

# Compiler
make

# Exécuter
./hotel_app
```

### Navigation

- **Flèches** : Naviguer
- **Enter** : Sélectionner
- **ESC** : Retour
- **Q** : Quitter
- **F1** : Aide

## 📁 Structure du Projet

```
.
├── ui/              # Modules UI
│   ├── ui.c         # Machine à états principale
│   ├── ui_draw.c    # Fonctions de dessin
│   ├── ui_input.c   # Gestion clavier
│   └── ...
├── src/             # Modules métier
│   ├── clients.c    # Gestion clients
│   ├── facturation.c # Facturation
│   └── ...
├── include/         # Headers
└── main.c          # Point d'entrée
```

## 🎯 Écrans Disponibles

1. **Dashboard** - Vue d'ensemble avec KPIs
2. **Clients** - Gestion des clients (CRUD)
3. **Rooms** - Gestion des chambres
4. **Reservations** - Gestion des réservations
5. **Billing** - Facturation et factures
6. **Help** - Aide et raccourcis

## 📖 Documentation Complète

Voir [TUI_DOCUMENTATION.md](TUI_DOCUMENTATION.md) pour la documentation détaillée.

## 🔧 Développement

### Compiler avec debug

```bash
make CFLAGS="-g -DDEBUG"
```

### Nettoyer

```bash
make clean
```

## 📝 Licence

Projet éducatif - Système de Gestion d'Hôtel


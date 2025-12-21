# Système de Gestion des Réservations d'Hôtel

## Project Overview (Shnou had l-projet)

Had system dyal gestion des réservations d'hôtel howa projet b C language li s7i7 o bhal, taydir gestion kamla l'hotel. Kayn fih les fonctionnalités li khasmin:

- **Gestion des réservations**: T'ajudi, t'modifi, o t'supprimer les réservations
- **Suivi des clients**: Gestion des informations dyal les clients
- **Disponibilité des chambres**: Vérification en temps réel dyal les chambres disponibles
- **Rapports et statistiques**: Génération des rapports pour la gestion

## Prerequisites (Shnou ghadi te7taj)

Bash t'khdm had l-application, khassk t'installi had les outils:

- **GCC Compiler** (MinGW/MSYS2) - Pour compiler le code C
- **CMake** - Pour la gestion du build system
- **Ncurses library** (Très important!) - Pour l'interface utilisateur en mode console

## Installation & Build (Kifash t-installi o t-khdem)

### Option A (Sahl) - Automatique

1. Double-click sur le fichier `setup.bat`
2. Suivez les instructions qui s'affichent
3. L'application sera compilée et prête à l'emploi

### Option B (Manuel) - Pour les avancés

Ouvrez le terminal et exécutez ces commandes:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
# Hotel Reservation System

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

## Project Overview

Marhaba! Hada howa projet dyal gestion des réservations d'hotel, maktoub b C language o kaykhdem b Ncurses bach ydir interface utilisateur f terminal. Kaydir gestion kamla dyal les réservations, les clients, o les chambres b tariqa sahla o efficace.

## Features

- **Gestion des réservations**: T'ajouti, t'modifi, o t'supprimi les réservations b souhula.
- **Suivi des clients**: Gestion des informations dyal les clients.
- **Disponibilité des chambres**: Vérification en temps réel dyal les chambres disponibles.
- **Facturation**: Gestion des factures o rapports.
- **Interface utilisateur**: TUI (Terminal User Interface) b Ncurses, facile l'utilisation.

## Prerequisites

Bash t'khdem had l-application, khassk t'installi had les outils:

- **GCC Compiler**: Pour compiler le code C.
- **CMake**: Pour la gestion du build system.
- **Ncurses**: Pour l'interface utilisateur en mode console.
  - Sur Linux: `libncurses-dev` (t'installi b `sudo apt install libncurses-dev`).
  - Sur Windows: PDCurses (kaykon disponible via vcpkg).
- **Make ou Ninja**: Pour l'exécution du build après CMake.

## Installation

1. Clone had l-repo o dir `cd` f dossier l-projet.
2. Créer dossier build: `mkdir build && cd build`.
3. Générer les fichiers build b CMake: `cmake ..`.
4. Compiler: `make` (o `ninja` si t'preferi).
5. Khdem l-application: `./hotel_reservation_system` (o smiya l'exécutable).

## Project Structure

Hada howa structure dyal l-projet, morganisi b dossiers logiques:

```
hotel_reservation_system/
├── src/                    # Logic principale
│   ├── main.c              # Point d'entrée dyal l'application
│   ├── room.c              # Gestion des chambres (ajout, modification, etc.)
│   ├── clients.c           # Gestion des clients (enregistrement, recherche)
│   ├── facturation.c       # Gestion des factures o paiements
│   ├── ui.c                # Dessin dyal l'interface Ncurses
│   └── data_init.c         # Initialisation des fichiers data
├── include/                # Headers (déclarations des fonctions)
│   ├── room.h
│   ├── clients.h
│   ├── facturation.h
│   ├── structures.h        # Structures de données
│   └── ...
├── data/                   # Bases de données runtime
│   ├── clients.dat         # Données des clients
│   ├── chambres.dat        # Données des chambres
│   ├── reservations.dat    # Données des réservations
│   └── factures.dat        # Données des factures
├── build/                  # Artifacts dyal compilation CMake
├── ui/                     # Composants UI supplémentaires
│   ├── ui_components.c     # Composants UI
│   ├── ui_draw.c           # Fonctions de dessin
│   ├── ui_input.c          # Gestion des inputs
│   └── ...
└── CMakeLists.txt          # Configuration CMake
```

## Usage

Après compilation, khdem `./hotel_reservation_system` f terminal. L'interface Ncurses ghaydirk naviguer b les menus pour gérer les réservations, les clients, o les chambres. Khassk t'followi les instructions f écran.

## Contributing

Si bghiti t'contribui, marhaba! Fork l-repo, dir tes changements, o submit pull request. Khassk t'followi les standards dyal code C o t'ajouti tests si possible.

## License

Hada l-projet howa open-source, sous license MIT. Khassk t'checki l-fichier LICENSE pour plus de détails.

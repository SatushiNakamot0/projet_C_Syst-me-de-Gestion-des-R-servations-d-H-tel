had l projet bach n5admouh ana ghadi n7at likoum structure , ntouma koulla ichouf chno 3jbo and tell achno bghito te5taro (ila ma3ejbatkoumch chi 7aja tell me bach nbaddelha ):

hotel_management/
│
├── main.c                    # Programme principal
├── structures.h              # Définitions des structures
├── fichiers.h/.c            # Gestion des fichiers
├── clients.h/.c             # gestion des clients Dounia atkhdem ela hd 
├── chambres.h/.c            # Module gestion chambres
├── reservations.h/.c        # Module gestion réservations
├── facturation.h/.c         #  Module de gestion de facturation kaoutar ghatkhdem 3la haada
├── menu.h/.c                # Interface console
├── validation.h/.c          # Validation des données
│
└── data/                    # Dossier pour les fichiers de données
    ├── clients.dat
    ├── chambres.dat
    ├── reservations.dat
    └── factures.dat


choufo hadi structure jdida choufo wach 3ejbatkoum bach nbda n9adha 9bl matebdaw tcodiw bach tel9aw koulchi wajed sf  :


hotel_management/
│
├── Makefile                  # Compiles the whole project with one command   
├── main.c                    # Entry point: just loads data and calls menu()
│
├── include/                  # All your .h files (Headers)
│   ├── structures.h          # The "Contract": Definitions of Client, Room, Reservation
│   ├── clients.h             # Prototypes for client management      hadi dyal Dounia 
│   ├── chambres.h            # Prototypes for room management
│   ├── reservations.h        # Prototypes for reservations
│   ├── facturation.h         # Prototypes for invoicing (Kaoutar's module)     Kaoutar l ostora atekhdem 3la hadi 
│   ├── menu.h                # Prototypes for the console UI
│   ├── utils.h               # Date tools and string helpers
│   └── fichiers.h            # Generic file handling (Load/Save)
│
├── src/                      # All your .c files (Implementation)
│   ├── clients.c             # Code for adding/searching clients    Dounia at5edmi 3la hadi 
│   ├── chambres.c            # Code for room management
│   ├── reservations.c        # Code for check-in/out logic
│   ├── facturation.c         # Code for invoice generation    Kaoutar at5edmi 3la hadi 7tta hiya sf 
│   ├── menu.c                # The switch/case menu logic
│   ├── utils.c               # Date calculation logic
│   └── fichiers.c            # File I/O logic
│
└── data/                     # Where the program stores data
    ├── clients.dat
    ├── chambres.dat
    ├── reservations.dat
    └── factures.dat

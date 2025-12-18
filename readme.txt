structure dyal l projet dyalna


hotel_management/
│
├── Makefile                  # Compiles the whole project with one command   
├── main.c                    # Entry point: just loads data and calls menu()
│
├── include/                  # All your .h files (Headers)
│   ├── structures.h          # The "Contract": Definitions of Client, Room, Reservation
│   ├── clients.h             # Prototypes for client management      dounia : salit
│   ├── chambres.h            # Prototypes for room management      yazid kheddam 3la hadi 
│   ├── reservations.h        # Prototypes for reservations        Dounia : ankhdem ela hd 
│   ├── facturation.h         # Prototypes for invoicing (Kaoutar's module)     Kaoutar l ostora atekhdem 3la hadi 
│   ├── menu.h                # Prototypes for the console UI
│   ├── utils.h               # Date tools and string helpers
│   └── fichiers.h            # Generic file handling (Load/Save)
│
├── src/                      # All your .c files (Implementation)
│   ├── clients.c             # Code for adding/searching clients    DOUNIA : salittt
│   ├── chambres.c            # Code for room management             yazid kheddam 3la hadi  
│   ├── reservations.c        # Code for check-in/out logic          Dounia : ankhdem ela hd 
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



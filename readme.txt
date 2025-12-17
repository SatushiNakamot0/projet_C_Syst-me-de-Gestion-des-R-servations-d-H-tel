structure dyal l projet dyalna


hotel_management/
│
├── Makefile                  # Compiles the whole project with one command   
├── main.c                    # Entry point: just loads data and calls menu()
│
├── include/                  # All your .h files (Headers)
│   ├── structures.h          # The "Contract": Definitions of Client, Room, Reservation
│   ├── clients.h             # Prototypes for client management      hadi dyal Dounia 
│   ├── chambres.h            # Prototypes for room management      yazid kheddam 3la hadi 
│   ├── reservations.h        # Prototypes for reservations        
│   ├── facturation.h         # Prototypes for invoicing (Kaoutar's module)     Kaoutar l ostora atekhdem 3la hadi 
│   ├── menu.h                # Prototypes for the console UI
│   ├── utils.h               # Date tools and string helpers
│   └── fichiers.h            # Generic file handling (Load/Save)
│
├── src/                      # All your .c files (Implementation)
│   ├── clients.c             # Code for adding/searching clients    Dounia at5edmi 3la hadi 
│   ├── chambres.c            # Code for room management             yazid kheddam 3la hadi  
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



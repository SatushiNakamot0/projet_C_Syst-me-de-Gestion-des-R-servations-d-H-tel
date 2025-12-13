#ifndef FICHIERS_H
#define FICHIERS_H

// Prototypes for file management functions
void sauvegarder_clients(Client *clients, int count);
void charger_clients(Client *clients, int *count);
void sauvegarder_chambres(Chambre *chambres, int count);
void charger_chambres(Chambre *chambres, int *count);
void sauvegarder_reservations(Reservation *reservations, int count);
void charger_reservations(Reservation *reservations, int *count);
void sauvegarder_factures(Facture *factures, int count);
void charger_factures(Facture *factures, int *count);

#endif

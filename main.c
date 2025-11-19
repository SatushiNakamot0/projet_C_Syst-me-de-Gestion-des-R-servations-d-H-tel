#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "fichiers.h"

int main()
{
    printf("Bienvenue dans le Système de Gestion des Réservations d’Hôtel\n");

    // Load data
    Client clients[100];
    int client_count = 0;
    charger_clients(clients, &client_count);

    Chambre chambres[50];
    int chambre_count = 0;
    charger_chambres(chambres, &chambre_count);

    Reservation reservations[100];
    int reservation_count = 0;
    charger_reservations(reservations, &reservation_count);

    Facture factures[100];
    int facture_count = 0;
    charger_factures(factures, &facture_count);

    // TODO: Implement menu system

    printf("Programme terminé.\n");
    return 0;
}

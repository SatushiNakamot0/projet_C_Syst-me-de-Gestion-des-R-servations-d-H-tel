#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct
{
    int id;
    char nom[50];
    char prenom[50];
    char email[100];
    char telephone[20];
} Client;

typedef struct
{
    int numero;
    char type[20];
    float prix;
    int disponible;
} Chambre;

typedef struct
{
    int id;
    int client_id;
    int chambre_numero;
    char date_debut[11];
    char date_fin[11];
    float montant;
} Reservation;

typedef struct
{
    int id;
    int reservation_id;
    float montant_total;
    char date_facturation[11];
} Facture;

#endif

#ifndef STRUCTURES_H
#define STRUCTURES_H

/* Constantes pour les limites */
#define MAX_CLIENTS 100
#define MAX_CHAMBRES 50
#define MAX_RESERVATIONS 100
#define MAX_FACTURES 100

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
    char statut[20];
} Reservation;

typedef struct Facture
{
    int idFacture;
    int idClient;
    int nbNuits;
    float prixNuit;
    float total;
} Facture;

#endif

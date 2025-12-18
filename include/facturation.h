#ifndef FACTURATION_H
#define FACTURATION_H

#define MAX_FACTURE 100

/* Structure représentant une facture */
typedef struct {
int numeroFacture;
char nomClient[50];
int nbNuits;
float prixNuit;
float total;
} Facture;

/* Prototypes des fonctions */
float calculerTotal(int nbNuits, float prixNuit);
void creerFacture(Facture *f);
void afficherFacture(Facture f);
void enregistrerFacture(Facture f);

#endif
#ifndef FACTURATION_H
#define FACTURATION_H

<<<<<<< HEAD
#include "structures.h"  // pour la structure Client et Facture

#define MAX_FACTURES 100  // Nombre maximum de factures

// Structure représentant une facture
typedef struct {
    int idFacture;     // Identifiant unique de la facture
    int idClient;      // Référence au client (via son ID)
    int nbNuits;       // Nombre de nuits
    float prixNuit;    // Prix par nuit
    float total;       // Montant total
} Facture;

/* Prototypes des fonctions */

// Calcul du total d'une facture
float calculer_total(int nbNuits, float prixNuit);

// Création d'une facture et ajout dans le tableau
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients);

// Affichage des factures
void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients);

// Sauvegarde des factures dans un fichier texte
void sauvegarder_factures(const Facture factures[], int count);
=======
#include "structures.h"

/* Prototypes des fonctions */
float calculer_total(int nbNuits, float prixNuit);

void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients);

void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients);

void sauvegarder_factures(Facture *factures, int count);
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5

#endif
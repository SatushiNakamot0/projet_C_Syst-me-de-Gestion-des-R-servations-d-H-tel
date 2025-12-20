#ifndef FACTURATION_H
#define FACTURATION_H

#include "structures.h"

/* Prototypes des fonctions */
float calculer_total(int nbNuits, float prixNuit);

void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients);

void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients);

void sauvegarder_factures(const Facture factures[], int count);

#endif
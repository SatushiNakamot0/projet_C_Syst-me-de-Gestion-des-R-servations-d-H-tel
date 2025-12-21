#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/structures.h"
#include "../include/facturation.h"
#include "../include/fichiers.h"

/* Déclaration des fonctions */
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients);
void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients);

/* Calcul du montant total */
float calculer_total(int nbNuits, float prixNuit)
{
    return nbNuits * prixNuit;
}

/* Création d'une facture */
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients)
{
    Facture nouvelle_facture;
    int id_client;
    int client_trouve = 0;
    int max_id = 0;

    if (*count >= MAX_FACTURES)
    {
        printf("\nErreur : Limite de factures atteinte.\n");
        return;
    }

    if (nbClients == 0)
    {
        printf("\nErreur : Aucun client enregistre.\n");
        return;
    }

    printf("\n--- CREATION D'UNE FACTURE ---\n");
    printf("Entrez l'ID du client : ");
    if (scanf("%d", &id_client) != 1)
        return;

    for (int i = 0; i < nbClients; i++)
    {
        if (clients[i].id == id_client)
        {
            client_trouve = 1;
            break;
        }
    }

    if (!client_trouve)
    {
        printf("Erreur : Client introuvable.\n");
        return;
    }

    printf("Nombre de nuits : ");
    scanf("%d", &nouvelle_facture.nbNuits);

    printf("Prix par nuit : ");
    scanf("%f", &nouvelle_facture.prixNuit);

    nouvelle_facture.total = calculer_total(
        nouvelle_facture.nbNuits,
        nouvelle_facture.prixNuit);

    for (int i = 0; i < *count; i++)
    {
        if (factures[i].idFacture > max_id)
            max_id = factures[i].idFacture;
    }

    nouvelle_facture.idFacture = max_id + 1;
    nouvelle_facture.idClient = id_client;

    factures[*count] = nouvelle_facture;
    (*count)++;

    sauvegarder_factures(factures, *count);

    printf("\nFacture creee avec succes (ID Facture: %d)\n", nouvelle_facture.idFacture);
}

/* Affichage des factures */
void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients)
{
    if (count == 0)
    {
        printf("\nAucune facture enregistree.\n");
        return;
    }

    printf("\n================================================================================\n");
    printf(" LISTE DES FACTURES (%d)\n", count);
    printf("================================================================================\n");

    for (int i = 0; i < count; i++)
    {
        const char *nom = "Inconnu";
        const char *prenom = "";

        for (int j = 0; j < nbClients; j++)
        {
            if (clients[j].id == factures[i].idClient)
            {
                nom = clients[j].nom;
                prenom = clients[j].prenom;
                break;
            }
        }

        printf("Facture ID: %d | Client: %s %s | Nuits: %d | Prix/Nuit: %.2f | Total: %.2f\n",
               factures[i].idFacture,
               prenom,
               nom,
               factures[i].nbNuits,
               factures[i].prixNuit,
               factures[i].total);
    }

    printf("================================================================================\n");
}
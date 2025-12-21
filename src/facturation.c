#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/structures.h"
#include "../include/facturation.h"
#include "../include/fichiers.h"
#include "../include/safe_input.h"
#include "../include/debug.h"

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

    LOG_INFO("Début de la création d'une facture");

    if (*count >= MAX_FACTURES)
    {
        LOG_ERROR("Limite de factures atteinte (%d)", MAX_FACTURES);
        printf("\nErreur : Limite de factures atteinte.\n");
        return;
    }

    if (nbClients == 0)
    {
        LOG_ERROR("Aucun client enregistré pour la facturation");
        printf("\nErreur : Aucun client enregistre.\n");
        return;
    }

    printf("\n--- CREATION D'UNE FACTURE ---\n");
    id_client = safe_input_int("Entrez l'ID du client : ");
    LOG_DEBUG("ID client pour facture: %d", id_client);

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
        LOG_ERROR("Client introuvable pour facturation: ID %d", id_client);
        printf("Erreur : Client introuvable.\n");
        return;
    }

    nouvelle_facture.nbNuits = safe_input_int("Nombre de nuits : ");
    LOG_DEBUG("Nombre de nuits: %d", nouvelle_facture.nbNuits);

    nouvelle_facture.prixNuit = safe_input_float("Prix par nuit : ");
    LOG_DEBUG("Prix par nuit: %.2f", nouvelle_facture.prixNuit);

    nouvelle_facture.total = calculer_total(
        nouvelle_facture.nbNuits,
        nouvelle_facture.prixNuit);
    
    LOG_DEBUG("Total calculé: %.2f", nouvelle_facture.total);

    /* Trouver le prochain ID de facture */
    for (int i = 0; i < *count; i++)
    {
        if (factures[i].idFacture > max_id)
            max_id = factures[i].idFacture;
    }

    nouvelle_facture.idFacture = max_id + 1;
    nouvelle_facture.idClient = id_client;

    factures[*count] = nouvelle_facture;
    (*count)++;

    LOG_INFO("Facture créée avec succès: ID %d, Client %d, Total %.2f", 
             nouvelle_facture.idFacture, id_client, nouvelle_facture.total);
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
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

/* Fonction pour récupérer le nom complet du client */
void get_nom_client(int idClient, const Client clients[], int nbClients, char *nom, char *prenom) {
    strcpy(nom, "Inconnu");
    strcpy(prenom, "");
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].id == idClient) {
            strcpy(nom, clients[i].nom);
            strcpy(prenom, clients[i].prenom);
            break;
        }
    }
}

/* Création d'une facture */
<<<<<<< HEAD
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients) {
    if (*count >= MAX_FACTURES) {
=======
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients)
{
    Facture nouvelle_facture;
    int id_client;
    int client_trouve = 0;
    int max_id = 0;

    LOG_INFO("Début de la création d'une facture");

    if (*count >= MAX_FACTURES)
    {
<<<<<<< HEAD
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5
=======
        LOG_ERROR("Limite de factures atteinte (%d)", MAX_FACTURES);
>>>>>>> c9edf7fd89baac70edcece9ba3b4cfbcc9b40868
        printf("\nErreur : Limite de factures atteinte.\n");
        return;
    }

    if (nbClients == 0)
    {
        LOG_ERROR("Aucun client enregistré pour la facturation");
        printf("\nErreur : Aucun client enregistre.\n");
        return;
    }

    Facture nouvelle_facture;
    int id_client;
    int client_trouve = 0;
    int max_id = 0;

    printf("\n--- CREATION D'UNE FACTURE ---\n");
<<<<<<< HEAD

    // Saisie ID client
    printf("Entrez l'ID du client : ");
<<<<<<< HEAD
    if (scanf("%d", &id_client) != 1) {
        printf("Erreur : saisie invalide.\n");
        while(getchar() != '\n');
        return;
    }
=======
    if (scanf("%d", &id_client) != 1)
        return;
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5
=======
    id_client = safe_input_int("Entrez l'ID du client : ");
    LOG_DEBUG("ID client pour facture: %d", id_client);
>>>>>>> c9edf7fd89baac70edcece9ba3b4cfbcc9b40868

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

<<<<<<< HEAD
    // Saisie nombre de nuits
    printf("Nombre de nuits : ");
    if (scanf("%d", &nouvelle_facture.nbNuits) != 1 || nouvelle_facture.nbNuits <= 0) {
        printf("Erreur : nombre de nuits invalide.\n");
        while(getchar() != '\n');
        return;
    }

    // Saisie prix par nuit
    printf("Prix par nuit : ");
    if (scanf("%f", &nouvelle_facture.prixNuit) != 1 || nouvelle_facture.prixNuit <= 0) {
        printf("Erreur : prix par nuit invalide.\n");
        while(getchar() != '\n');
        return;
    }
=======
    nouvelle_facture.nbNuits = safe_input_int("Nombre de nuits : ");
    LOG_DEBUG("Nombre de nuits: %d", nouvelle_facture.nbNuits);

    nouvelle_facture.prixNuit = safe_input_float("Prix par nuit : ");
    LOG_DEBUG("Prix par nuit: %.2f", nouvelle_facture.prixNuit);
>>>>>>> c9edf7fd89baac70edcece9ba3b4cfbcc9b40868

<<<<<<< HEAD
    // Calcul du total
    nouvelle_facture.total = calculer_total(nouvelle_facture.nbNuits, nouvelle_facture.prixNuit);

    // Calcul ID facture
    for (int i = 0; i < *count; i++) {
=======
    nouvelle_facture.total = calculer_total(
        nouvelle_facture.nbNuits,
        nouvelle_facture.prixNuit);
    
    LOG_DEBUG("Total calculé: %.2f", nouvelle_facture.total);

    /* Trouver le prochain ID de facture */
    for (int i = 0; i < *count; i++)
    {
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5
        if (factures[i].idFacture > max_id)
            max_id = factures[i].idFacture;
    }
    nouvelle_facture.idFacture = max_id + 1;
    nouvelle_facture.idClient = id_client;

    // Ajout de la facture au tableau
    factures[*count] = nouvelle_facture;
    (*count)++;

<<<<<<< HEAD
    // Sauvegarde dans le fichier
=======
    LOG_INFO("Facture créée avec succès: ID %d, Client %d, Total %.2f", 
             nouvelle_facture.idFacture, id_client, nouvelle_facture.total);
>>>>>>> c9edf7fd89baac70edcece9ba3b4cfbcc9b40868
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

<<<<<<< HEAD
    for (int i = 0; i < count; i++) {
        char nom[50], prenom[50];
        get_nom_client(factures[i].idClient, clients, nbClients, nom, prenom);
=======
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
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5

        printf("Facture ID: %d | Client: %s %s | Nuits: %d | Prix/Nuit: %.2f | Total: %.2f\n",
               factures[i].idFacture,
               prenom,
               nom,
               factures[i].nbNuits,
               factures[i].prixNuit,
               factures[i].total);
    }

    printf("================================================================================\n");
<<<<<<< HEAD
}

/* Sauvegarde des factures dans un fichier */
void sauvegarder_factures(const Facture factures[], int count) {
    FILE *f = fopen("factures.txt", "w"); // écrase le fichier à chaque sauvegarde
    if (!f) {
        printf("Erreur ouverture fichier factures.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(f, "%d %d %d %.2f %.2f\n",
                factures[i].idFacture,
                factures[i].idClient,
                factures[i].nbNuits,
                factures[i].prixNuit,
                factures[i].total);
    }

    fclose(f);
}
=======
}
>>>>>>> 0323135ec3abbb04083ebca1aa1ee2302aa49ca5

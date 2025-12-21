#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "facturation.h"
#include "clients.h"
#include "structures.h"

/* Calcul du montant total */
float calculer_total(int nbNuits, float prixNuit) {
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
void creer_facture(Facture factures[], int *count, const Client clients[], int nbClients) {
    if (*count >= MAX_FACTURES) {
        printf("\nErreur : Limite de factures atteinte.\n");
        return;
    }

    if (nbClients == 0) {
        printf("\nErreur : Aucun client enregistre.\n");
        return;
    }

    Facture nouvelle_facture;
    int id_client;
    int client_trouve = 0;
    int max_id = 0;

    printf("\n--- CREATION D'UNE FACTURE ---\n");

    // Saisie ID client
    printf("Entrez l'ID du client : ");
    if (scanf("%d", &id_client) != 1) {
        printf("Erreur : saisie invalide.\n");
        while(getchar() != '\n');
        return;
    }

    for (int i = 0; i < nbClients; i++) {
        if (clients[i].id == id_client) {
            client_trouve = 1;
            break;
        }
    }

    if (!client_trouve) {
        printf("Erreur : Client introuvable.\n");
        return;
    }

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

    // Calcul du total
    nouvelle_facture.total = calculer_total(nouvelle_facture.nbNuits, nouvelle_facture.prixNuit);

    // Calcul ID facture
    for (int i = 0; i < *count; i++) {
        if (factures[i].idFacture > max_id)
            max_id = factures[i].idFacture;
    }
    nouvelle_facture.idFacture = max_id + 1;
    nouvelle_facture.idClient = id_client;

    // Ajout de la facture au tableau
    factures[*count] = nouvelle_facture;
    (*count)++;

    // Sauvegarde dans le fichier
    sauvegarder_factures(factures, *count);

    printf("\nFacture creee avec succes (ID Facture: %d)\n", nouvelle_facture.idFacture);
}

/* Affichage des factures */
void afficher_factures(const Facture factures[], int count, const Client clients[], int nbClients) {
    if (count == 0) {
        printf("\nAucune facture enregistree.\n");
        return;
    }

    printf("\n================================================================================\n");
    printf(" LISTE DES FACTURES (%d)\n", count);
    printf("================================================================================\n");

    for (int i = 0; i < count; i++) {
        char nom[50], prenom[50];
        get_nom_client(factures[i].idClient, clients, nbClients, nom, prenom);

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/clients.h"
#include "../include/structures.h"
#include "../include/fichiers.h"  // For sauvegarder_clients

void ajouter_client(Client clients[], int *count) {
    Client nouveau_client;
    char temp_email[100];
    int max_id = 0; 

    if (*count >= MAX_CLIENTS) {
        printf("\nErreur: Limite de clients atteinte (%d). Ajout impossible.\n", MAX_CLIENTS);
        return;
    }

    printf("\n--- AJOUT D'UN NOUVEAU CLIENT ---\n");

    int email_unique = 0;
    do {
        printf("Entrez l'Email (doit etre unique): ");
        if (scanf("%s", temp_email) != 1) { return; }

        email_unique = 1;
        for (int i = 0; i < *count; i++) {
            if (strcmp(clients[i].email, temp_email) == 0) {
                printf("Erreur: Cet email existe deja. Veuillez reessayer.\n");
                email_unique = 0;
                break;
            }
        }
    } while (email_unique == 0);

    printf("Entrez le Nom: ");
    scanf("%s", nouveau_client.nom);
    printf("Entrez le Prenom: ");
    scanf("%s", nouveau_client.prenom);
    printf("Entrez le Telephone: ");
    scanf("%s", nouveau_client.telephone);
    

    strcpy(nouveau_client.email, temp_email);

    for (int i = 0; i < *count; i++) {
        if (clients[i].id > max_id) {
            max_id = clients[i].id;
        }
    }
    nouveau_client.id = max_id + 1;

    clients[*count] = nouveau_client;
    (*count)++;


    sauvegarder_clients(clients, *count);

    printf("Client %s %s (ID %d) ajoute et sauvegarde avec succes.\n",
           nouveau_client.prenom, nouveau_client.nom, nouveau_client.id);
}


void afficher_clients(const Client clients[], int count) {
    if (count == 0) {
        printf("\n--- AUCUN CLIENT ENREGISTRE ---\n");
        return;
    }

    printf("\n======================================================================================\n");
    printf("  LISTE DES CLIENTS ACTUELS (%d)\n", count);
    printf("======================================================================================\n");
    printf("| ID | Nom | Prenom | Telephone | Email\n");
    printf("--------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("| %-4d | %-15s | %-15s | %-10s | %s\n",
               clients[i].id,
               clients[i].nom,
               clients[i].prenom,
               clients[i].telephone,
               clients[i].email);
    }
    printf("======================================================================================\n");
}


void modifier_client(Client clients[], int count) {
    int id_a_modifier, index_client = -1;
    char temp_email[100];
    
    printf("\n--- MODIFICATION D'UN CLIENT ---\n");
    printf("Entrez l'ID du client a modifier: ");
    if (scanf("%d", &id_a_modifier) != 1) { return; }


    for (int i = 0; i < count; i++) {
        if (clients[i].id == id_a_modifier) {
            index_client = i;
            break;
        }
    }

    if (index_client == -1) {
        printf("Erreur: Client avec ID %d non trouve.\n", id_a_modifier);
        return;
    }

    
    printf("\nClient selectionne: ID %d | %s %s | Email: %s\n", 
           clients[index_client].id, clients[index_client].prenom, clients[index_client].nom, clients[index_client].email);
    
    int choix;
    printf("\nQue voulez-vous modifier ? (1: Nom/Prenom, 2: Telephone, 3: Email): ");
    if (scanf("%d", &choix) != 1) { return; }

    switch (choix) {
        case 1:
            printf("Nouveau Nom (actuel: %s): ", clients[index_client].nom);
            scanf("%s", clients[index_client].nom);
            printf("Nouveau Prenom (actuel: %s): ", clients[index_client].prenom);
            scanf("%s", clients[index_client].prenom);
            break;
        case 2:
            printf("Nouveau Telephone (actuel: %s): ", clients[index_client].telephone);
            scanf("%s", clients[index_client].telephone);
            break;
        case 3:
            int email_unique;
            do {
                printf("Nouvel Email (actuel: %s): ", clients[index_client].email);
                scanf("%s", temp_email);

                email_unique = 1;
                for (int i = 0; i < count; i++) {
                    if (i != index_client && strcmp(clients[i].email, temp_email) == 0) {
                        printf("Erreur: Cet email existe deja pour un autre client. Reessayez.\n");
                        email_unique = 0;
                        break;
                    }
                }
            } while (email_unique == 0);
            strcpy(clients[index_client].email, temp_email);
            break;
        default:
            printf("Choix invalide. Aucune modification effectuee.\n");
            return;
    }


    sauvegarder_clients(clients, count);
    printf("\nClient %d modifie et sauvegarde avec succes.\n", id_a_modifier);
}


void rechercher_client_par_nom(const Client clients[], int count) {
    char nom_recherche[50];
    int trouve = 0;

    printf("\n--- RECHERCHE DE CLIENT PAR NOM ---\n");
    printf("Entrez le nom ou une partie du nom a rechercher: ");
    scanf("%s", nom_recherche);

    printf("\nResultats pour '%s':\n", nom_recherche);
    printf("------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        if (strstr(clients[i].nom, nom_recherche) != NULL ||
            strstr(clients[i].prenom, nom_recherche) != NULL) {

            printf("ID: %d | Nom: %s %s | Email: %s | Tel: %s\n",
                   clients[i].id, clients[i].prenom, clients[i].nom, clients[i].email, clients[i].telephone);
            trouve = 1;
        }
    }

    if (!trouve) {
        printf("Aucun client trouve pour '%s'.\n", nom_recherche);
    }
    printf("------------------------------------------------------\n");
}


void supprimer_client(Client clients[], int *count) {
    int id_a_supprimer, index_client = -1;

    printf("\n--- SUPPRESSION D'UN CLIENT ---\n");
    printf("Entrez l'ID du client a supprimer: ");
    if (scanf("%d", &id_a_supprimer) != 1) { return; }

    for (int i = 0; i < *count; i++) {
        if (clients[i].id == id_a_supprimer) {
            index_client = i;
            break;
        }
    }

    if (index_client == -1) {
        printf("Erreur: Client avec ID %d non trouve.\n", id_a_supprimer);
        return;
    }


    printf("\nConfirmez-vous la suppression du client ID %d, Nom: %s %s ? (o/n): ", 
           id_a_supprimer, clients[index_client].prenom, clients[index_client].nom);
    char confirmation;
    scanf(" %c", &confirmation);

    if (confirmation != 'o' && confirmation != 'O') {
        printf("Suppression annulee.\n");
        return;
    }

    for (int i = index_client; i < (*count) - 1; i++) {
        clients[i] = clients[i + 1];
    }

    (*count)--;
 
    sauvegarder_clients(clients, *count);
    printf("\nClient %d supprime et sauvegarde avec succes.\n", id_a_supprimer);
}
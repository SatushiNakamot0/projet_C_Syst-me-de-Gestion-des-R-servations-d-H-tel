#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/clients.h"
#include "../include/structures.h"
#include "../include/fichiers.h"  // For sauvegarder_clients
#include "../include/safe_input.h"
#include "../include/debug.h"

void ajouter_client(Client clients[], int *count) {
    Client nouveau_client;
    char temp_email[MAX_EMAIL_LENGTH];
    int max_id = 0; 

    LOG_INFO("Début de l'ajout d'un nouveau client");

    // N-checkiw ila kan 3endo l-space f l-array
    if (*count >= MAX_CLIENTS) {
        LOG_ERROR("Limite de clients atteinte (%d)", MAX_CLIENTS);
        printf("\nErreur: Limite de clients atteinte (%d). Ajout impossible.\n", MAX_CLIENTS);
        return;
    }

    printf("\n--- AJOUT D'UN NOUVEAU CLIENT ---\n");

    // Kan-demandiw l-email w kan-vérifiiw ila kan unique
    int email_unique = 0;
    do {
        safe_input_string("Entrez l'Email (doit etre unique): ", temp_email, MAX_EMAIL_LENGTH);
        LOG_DEBUG("Email saisi: %s", temp_email);

        email_unique = 1;
        // Hna kan-checkiw ila l-email deja kayn
        for (int i = 0; i < *count; i++) {
            if (strcmp(clients[i].email, temp_email) == 0) {
                printf("Erreur: Cet email existe deja. Veuillez reessayer.\n");
                LOG_ERROR("Email dupliqué: %s", temp_email);
                email_unique = 0;
                break;
            }
        }
    } while (email_unique == 0);

    // Kan-demandiw les informations dyal l-client
    safe_input_string("Entrez le Nom: ", nouveau_client.nom, MAX_NAME_LENGTH);
    safe_input_string("Entrez le Prenom: ", nouveau_client.prenom, MAX_NAME_LENGTH);
    safe_input_string("Entrez le Telephone: ", nouveau_client.telephone, MAX_PHONE_LENGTH);
    
    LOG_DEBUG("Données client - Nom: %s, Prenom: %s, Tel: %s", 
              nouveau_client.nom, nouveau_client.prenom, nouveau_client.telephone);

    strcpy(nouveau_client.email, temp_email);

    // Hna kan-cherchiw l-ID l-kbir w kan-zidiw 1
    for (int i = 0; i < *count; i++) {
        if (clients[i].id > max_id) {
            max_id = clients[i].id;
        }
    }
    nouveau_client.id = max_id + 1;

    // Kan-ajoutiw l-client l-array w kan-sauvegardiw
    clients[*count] = nouveau_client;
    (*count)++;

    LOG_INFO("Client ajouté avec succès: ID %d", nouveau_client.id);
    sauvegarder_clients(clients, *count);

    printf("Client %s %s (ID %d) ajouté et sauvegardé avec succès.\n",
           nouveau_client.prenom, nouveau_client.nom, nouveau_client.id);
}


void afficher_clients(const Client clients[], int count) {
    // N-checkiw ila kan 3endo chi client
    if (count == 0) {
        printf("\n--- AUCUN CLIENT ENREGISTRE ---\n");
        return;
    }

    // Hna kan-affichiw l-header dyal l-tableau
    printf("\n======================================================================================\n");
    printf("  LISTE DES CLIENTS ACTUELS (%d)\n", count);
    printf("======================================================================================\n");
    printf("| ID | Nom | Prenom | Telephone | Email\n");
    printf("--------------------------------------------------------------------------------------\n");

    // Kan-loopiw 3la ga3 les clients w kan-affichiwhom
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
    char temp_email[MAX_EMAIL_LENGTH];
    
    LOG_INFO("Début de la modification d'un client");
    
    printf("\n--- MODIFICATION D'UN CLIENT ---\n");
    id_a_modifier = safe_input_int("Entrez l'ID du client a modifier: ");
    LOG_DEBUG("ID à modifier: %d", id_a_modifier);

    // Kan-cherchiw l-client b l-ID
    for (int i = 0; i < count; i++) {
        if (clients[i].id == id_a_modifier) {
            index_client = i;
            break;
        }
    }

    if (index_client == -1) {
        LOG_ERROR("Client non trouvé: ID %d", id_a_modifier);
        printf("Erreur: Client avec ID %d non trouve.\n", id_a_modifier);
        return;
    }

    
    printf("\nClient selectionne: ID %d | %s %s | Email: %s\n", 
           clients[index_client].id, clients[index_client].prenom, clients[index_client].nom, clients[index_client].email);
    
    int choix;
    printf("\nQue voulez-vous modifier ? (1: Nom/Prenom, 2: Telephone, 3: Email): ");
    choix = safe_input_int("");

    // Kan-switchiw 3la l-choix dyal l-user
    switch (choix) {
        case 1:
            // Modification dyal nom w prenom
            safe_input_string("Nouveau Nom (actuel: %s): ", clients[index_client].nom, MAX_NAME_LENGTH);
            safe_input_string("Nouveau Prenom (actuel: %s): ", clients[index_client].prenom, MAX_NAME_LENGTH);
            break;
        case 2:
            // Modification dyal telephone
            safe_input_string("Nouveau Telephone (actuel: %s): ", clients[index_client].telephone, MAX_PHONE_LENGTH);
            break;
        case 3:
            // Modification dyal email b verification dyal l-uniqueness
            int email_unique;
            do {
                safe_input_string("Nouvel Email (actuel: %s): ", temp_email, MAX_EMAIL_LENGTH);
                LOG_DEBUG("Nouvel email: %s", temp_email);

                email_unique = 1;
                // Kan-checkiw ila l-email deja kayn 3nd chi client akhor
                for (int i = 0; i < count; i++) {
                    if (i != index_client && strcmp(clients[i].email, temp_email) == 0) {
                        printf("Erreur: Cet email existe deja pour un autre client. Reessayez.\n");
                        LOG_ERROR("Email dupliqué: %s", temp_email);
                        email_unique = 0;
                        break;
                    }
                }
            } while (email_unique == 0);
            strcpy(clients[index_client].email, temp_email);
            break;
        default:
            LOG_ERROR("Choix invalide: %d", choix);
            printf("Choix invalide. Aucune modification effectuee.\n");
            return;
    }

    LOG_INFO("Client %d modifié avec succès", id_a_modifier);
    sauvegarder_clients(clients, count);
    printf("\nClient %d modifie et sauvegarde avec succes.\n", id_a_modifier);
}


void rechercher_client_par_nom(const Client clients[], int count) {
    char nom_recherche[MAX_NAME_LENGTH];
    int trouve = 0;

    LOG_INFO("Début de la recherche de client par nom");
    
    printf("\n--- RECHERCHE DE CLIENT PAR NOM ---\n");
    safe_input_string("Entrez le nom ou une partie du nom a rechercher: ", nom_recherche, MAX_NAME_LENGTH);
    LOG_DEBUG("Terme recherché: %s", nom_recherche);

    printf("\nResultats pour '%s':\n", nom_recherche);
    printf("------------------------------------------------------\n");

    // Kan-loopiw 3la ga3 les clients w kan-recherchiw
    for (int i = 0; i < count; i++) {
        // N-checkiw ila l-kalma kayna f nom ola prenom
        if (strstr(clients[i].nom, nom_recherche) != NULL ||
            strstr(clients[i].prenom, nom_recherche) != NULL) {

            printf("ID: %d | Nom: %s %s | Email: %s | Tel: %s\n",
                   clients[i].id, clients[i].prenom, clients[i].nom, clients[i].email, clients[i].telephone);
            trouve = 1;
        }
    }

    if (!trouve) {
        printf("Aucun client trouve pour '%s'.\n", nom_recherche);
        LOG_INFO("Aucun résultat trouvé pour: %s", nom_recherche);
    } else {
        LOG_INFO("Recherche terminée - %d résultats trouvés", trouve);
    }
    printf("------------------------------------------------------\n");
}


void supprimer_client(Client clients[], int *count) {
    int id_a_supprimer, index_client = -1;

    LOG_INFO("Début de la suppression d'un client");
    
    printf("\n--- SUPPRESSION D'UN CLIENT ---\n");
    id_a_supprimer = safe_input_int("Entrez l'ID du client a supprimer: ");
    LOG_DEBUG("ID à supprimer: %d", id_a_supprimer);

    // Kan-cherchiw l-client b l-ID
    for (int i = 0; i < *count; i++) {
        if (clients[i].id == id_a_supprimer) {
            index_client = i;
            break;
        }
    }

    if (index_client == -1) {
        LOG_ERROR("Client non trouvé pour suppression: ID %d", id_a_supprimer);
        printf("Erreur: Client avec ID %d non trouve.\n", id_a_supprimer);
        return;
    }

    printf("\nConfirmez-vous la suppression du client ID %d, Nom: %s %s ? (o/n): ", 
           id_a_supprimer, clients[index_client].prenom, clients[index_client].nom);
    
    int confirmation = safe_input_yes_no("");

    if (!confirmation) {
        LOG_INFO("Suppression annulée par l'utilisateur: ID %d", id_a_supprimer);
        printf("Suppression annulee.\n");
        return;
    }

    // Hna kan-décaliw les éléments bach n-combliw l-vide
    for (int i = index_client; i < (*count) - 1; i++) {
        clients[i] = clients[i + 1];
    }

    (*count)--;
 
    LOG_INFO("Client %d supprimé avec succès", id_a_supprimer);
    sauvegarder_clients(clients, *count);
    printf("\nClient %d supprime et sauvegarde avec succes.\n", id_a_supprimer);
}
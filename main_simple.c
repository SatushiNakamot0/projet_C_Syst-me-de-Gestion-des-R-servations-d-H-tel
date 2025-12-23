#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/structures.h"
#include "../include/clients.h"
#include "../include/chambres.h"
#include "../include/reservations.h"
#include "../include/facturation.h"

/* Global variables */
Client clients[MAX_CLIENTS];
int nb_clients = 0;
Chambre chambres[MAX_CHAMBRES];
int nb_chambres = 0;
Reservation reservations[MAX_RESERVATIONS];
int nb_reservations = 0;
Facture factures[MAX_FACTURES];
int nb_factures = 0;

void afficher_menu_principal() {
    printf("\n=== SYSTÈME DE GESTION D'HÔTEL ===\n");
    printf("1. Gestion des clients\n");
    printf("2. Gestion des chambres\n");
    printf("3. Gestion des réservations\n");
    printf("4. Gestion des factures\n");
    printf("0. Quitter\n");
    printf("Choix : ");
}

void afficher_sous_menu(const char *titre) {
    printf("\n=== %s ===\n", titre);
    printf("1. Ajouter\n");
    printf("2. Afficher\n");
    printf("3. Modifier\n");
    printf("4. Supprimer\n");
    printf("0. Retour\n");
    printf("Choix : ");
}

void gerer_menu_clients() {
    int choix;
    do {
        afficher_sous_menu("GESTION DES CLIENTS");
        scanf("%d", &choix);
        getchar(); // Pour consommer le '\n' laissé par scanf
        
        switch(choix) {
            case 1:
                //ajouter_client(clients, &nb_clients);
                printf("Ajout d'un client\n");
                break;
            case 2:
                //afficher_clients(clients, nb_clients);
                printf("Affichage des clients\n");
                break;
            case 0:
                printf("Retour au menu principal.\n");
                break;
            default:
                printf("Option non valide.\n");
        }
    } while(choix != 0);
}

void gerer_menu_chambres() {
    int choix;
    do {
        afficher_sous_menu("GESTION DES CHAMBRES");
        scanf("%d", &choix);
        getchar();
        
        switch(choix) {
            case 1:
                printf("Ajout d'une chambre\n");
                break;
            case 2:
                printf("Affichage des chambres\n");
                break;
            case 0:
                printf("Retour au menu principal.\n");
                break;
            default:
                printf("Option non valide.\n");
        }
    } while(choix != 0);
}

void gerer_menu_reservations() {
    int choix;
    do {
        afficher_sous_menu("GESTION DES RÉSERVATIONS");
        scanf("%d", &choix);
        getchar();
        
        switch(choix) {
            case 1:
                printf("Ajout d'une réservation\n");
                break;
            case 2:
                printf("Affichage des réservations\n");
                break;
            case 0:
                printf("Retour au menu principal.\n");
                break;
            default:
                printf("Option non valide.\n");
        }
    } while(choix != 0);
}

void gerer_menu_factures() {
    int choix;
    do {
        afficher_sous_menu("GESTION DES FACTURES");
        scanf("%d", &choix);
        getchar();
        
        switch(choix) {
            case 1:
                creer_facture(factures, &nb_factures, clients, nb_clients);
                break;
            case 2:
                afficher_factures(factures, nb_factures, clients, nb_clients);
                break;
            case 0:
                printf("Retour au menu principal.\n");
                break;
            default:
                printf("Option non valide.\n");
        }
    } while(choix != 0);
}

int main() {
    // Charger les données depuis les fichiers si nécessaire
    // charger_clients(clients, &nb_clients);
    // charger_chambres(chambres, &nb_chambres);
    // ... autres chargements

    int choix;
    do {
        afficher_menu_principal();
        scanf("%d", &choix);
        getchar(); // Pour consommer le '\n' laissé par scanf
        
        switch(choix) {
            case 1:
                gerer_menu_clients();
                break;
            case 2:
                gerer_menu_chambres();
                break;
            case 3:
                gerer_menu_reservations();
                break;
            case 4:
                gerer_menu_factures();
                break;
            case 0:
                printf("Au revoir !\n");
                break;
            default:
                printf("Option non valide.\n");
        }
    } while(choix != 0);

    // Sauvegarder les données dans les fichiers si nécessaire
    // sauvegarder_clients(clients, nb_clients);
    // sauvegarder_chambres(chambres, nb_chambres);
    // ... autres sauvegardes

    return 0;
}

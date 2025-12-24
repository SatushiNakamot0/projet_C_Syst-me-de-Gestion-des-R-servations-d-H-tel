#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/room.h"
#include "../include/debug.h"
#include "../include/safe_input.h"

/* Constants dyal validation */
#define MAX_ROOMS 100
#define MIN_PRICE 0.0f
#define MAX_PRICE 10000.0f
#define ROOMS_FILE_PATH "data/rooms.txt"

/* ============================================================================
 * IMPLEMENTATION DYAL GESTION DES ROOMS
 * ============================================================================
 * Hna kan-geriw les chambres b daba, b validation w error handling
 * Kan-protegiw l-data mn l-corruption w kan-logiw l-errors
 * ============================================================================ */

/* Validation dyal room data */
int validate_room_data(const Room *room) {
    if (!room) {
        LOG_ERROR("Pointeur de room invalide");
        return 0; // Pointeur invalide
    }
    
    // N-validiw l-ID
    if (room->id <= 0) {
        LOG_ERROR("ID de chambre invalide: %d", room->id);
        return 0; // ID invalide
    }
    
    // N-validiw l-type dyal l-chambre
    if (strlen(room->type) == 0 || strlen(room->type) >= MAX_ROOM_TYPE_LENGTH) {
        LOG_ERROR("Type de chambre invalide: '%s' (longueur: %zu)", room->type, strlen(room->type));
        return 0; // Type invalide
    }
    
    // N-validiw l-prix
    if (room->price < MIN_PRICE || room->price > MAX_PRICE) {
        LOG_ERROR("Prix de chambre invalide: %.2f (doit être %.0f-%.0f)", 
                  room->price, MIN_PRICE, MAX_PRICE);
        return 0; // Prix hors limites
    }
    
    // N-validiw l-status
    if (strcmp(room->status, "Available") != 0 && strcmp(room->status, "Occupied") != 0) {
        LOG_ERROR("Statut de chambre invalide: '%s'", room->status);
        return 0; // Statut invalide
    }
    
    LOG_DEBUG("Validation de chambre réussie: #%d %s", room->id, room->type);
    return 1; // L-chambre valide
}

/* Sauvegarde dyal room f fichier */
void save_room_to_file(const Room *room) {
    FILE *file = fopen(ROOMS_FILE_PATH, "a");
    if (!file) {
        LOG_ERROR("Impossible d'ouvrir le fichier %s", ROOMS_FILE_PATH);
        printf("Erreur: Impossible de sauvegarder la chambre.\n");
        return;
    }
    
    // Kan-ktibw l-room data f format CSV
    fprintf(file, "%d,%s,%.2f,%s\n", room->id, room->type, room->price, room->status);
    fclose(file);
    
    LOG_INFO("Chambre sauvegardée: #%d %s", room->id, room->type);
}

/* Load rooms mn fichier */
void load_rooms_from_file(Room *rooms, int *count) {
    FILE *file = fopen(ROOMS_FILE_PATH, "r");
    if (!file) {
        LOG_INFO("Fichier %s non trouvé, création d'un nouveau fichier", ROOMS_FILE_PATH);
        *count = 0;
        return;
    }
    
    *count = 0;
    char line[256];
    
    // Kan-ktibw ga3 les lignes mn fichier
    while (fgets(line, sizeof(line), file) && *count < MAX_ROOMS) {
        // N-parseiw l-line
        if (sscanf(line, "%d,%49[^,],%f,%19[^\n]", 
                   &rooms[*count].id, 
                   rooms[*count].type, 
                   &rooms[*count].price, 
                   rooms[*count].status) == 4) {
            (*count)++;
        } else {
            LOG_WARN("Ligne invalide dans le fichier des chambres: %s", line);
        }
    }
    
    fclose(file);
    LOG_INFO("Chargement de %d chambres depuis %s", *count, ROOMS_FILE_PATH);
}

/* Get next available ID */
int get_next_room_id() {
    Room rooms[MAX_ROOMS];
    int count = 0;
    int max_id = 0;
    
    load_rooms_from_file(rooms, &count);
    
    // Kan-cherchiw l-max ID
    for (int i = 0; i < count; i++) {
        if (rooms[i].id > max_id) {
            max_id = rooms[i].id;
        }
    }
    
    return max_id + 1;
}

/* Check ila ID unique */
int is_room_id_unique(int id) {
    Room rooms[MAX_ROOMS];
    int count = 0;
    
    load_rooms_from_file(rooms, &count);
    
    // Kan-checkiw ila l-ID kayn deja
    for (int i = 0; i < count; i++) {
        if (rooms[i].id == id) {
            return 0; // ID deja kayn
        }
    }
    
    return 1; // ID unique
}

/* ============================================================================ */
/* MAIN FUNCTIONS */
/* ============================================================================ */

void add_room(void) {
    Room new_room;
    int use_auto_id = 0;
    
    LOG_INFO("Début de l'ajout d'une nouvelle chambre");
    
    printf("\n--- AJOUT D'UNE NOUVELLE CHAMBRE ---\n");
    
    // Kan-demandiw ila bgha auto ID ola manual
    do {
        printf("Voulez-vous utiliser un ID automatique? (1=Oui, 0=Non): ");
        use_auto_id = safe_input_int("");
        
        if (use_auto_id != 0 && use_auto_id != 1) {
            printf("Erreur: Veuillez entrer 0 ou 1.\n");
        }
    } while (use_auto_id != 0 && use_auto_id != 1);
    
    if (use_auto_id) {
        // Auto-generate ID
        new_room.id = get_next_room_id();
        printf("ID automatique généré: %d\n", new_room.id);
        LOG_DEBUG("ID automatique: %d", new_room.id);
    } else {
        // Manual ID input
        int id_unique = 0;
        do {
            new_room.id = safe_input_int("Entrez l'ID de la chambre: ");
            LOG_DEBUG("ID saisi: %d", new_room.id);
            
            if (new_room.id <= 0) {
                printf("Erreur: L'ID doit être positif.\n");
                LOG_ERROR("ID invalide: %d", new_room.id);
                continue;
            }
            
            // Hna kan-checkiw ila l-ID unique
            if (!is_room_id_unique(new_room.id)) {
                printf("Erreur: L'ID %d existe déjà. Veuillez choisir un autre ID.\n", new_room.id);
                LOG_ERROR("ID de chambre dupliqué: %d", new_room.id);
            } else {
                id_unique = 1;
                LOG_DEBUG("ID de chambre valide: %d", new_room.id);
            }
        } while (!id_unique);
    }
    
    // Kan-demandiw l-type dyal l-chambre
    safe_input_string("Entrez le type de chambre (ex: Single, Double, Suite): ", new_room.type, MAX_ROOM_TYPE_LENGTH);
    LOG_DEBUG("Type de chambre: %s", new_room.type);
    
    // Kan-demandiw l-prix par nuit
    do {
        new_room.price = safe_input_float("Entrez le prix par nuit (EUR): ");
        LOG_DEBUG("Prix saisi: %.2f", new_room.price);
        
        if (new_room.price <= MIN_PRICE || new_room.price > MAX_PRICE) {
            printf("Erreur: Prix invalide (doit être entre %.0f et %.0f EUR).\n", MIN_PRICE, MAX_PRICE);
            LOG_ERROR("Prix hors limites: %.2f", new_room.price);
        }
    } while (new_room.price <= MIN_PRICE || new_room.price > MAX_PRICE);
    
    // L-chambres jdad kaynin "Available" par défaut
    strcpy(new_room.status, "Available");
    
    // Validation finale dyal ga3 les données
    if (!validate_room_data(&new_room)) {
        printf("Erreur: Données de chambre invalides.\n");
        LOG_ERROR("Validation échouée pour la chambre");
        return;
    }
    
    // Kan-sauvegardiw l-chambre
    save_room_to_file(&new_room);
    
    LOG_INFO("Chambre ajoutée avec succès: #%d %s (%.2f EUR)", 
             new_room.id, new_room.type, new_room.price);
    
    printf("Chambre %d (%s) ajoutée et sauvegardée avec succès.\n",
           new_room.id, new_room.type);
}

void list_rooms(void) {
    Room rooms[MAX_ROOMS];
    int count = 0;
    
    LOG_INFO("Début de l'affichage des chambres");
    
    // Kan-loadiw les chambres mn fichier
    load_rooms_from_file(rooms, &count);
    
    // N-checkiw ila kan 3endo chi chambre
    if (count == 0) {
        printf("\n--- AUCUNE CHAMBRE ENREGISTRÉE ---\n");
        LOG_INFO("Aucune chambre trouvée");
        return;
    }

    // Hna kan-affichiw l-header dyal l-tableau
    printf("\n======================================================================================\n");
    printf("  LISTE DES CHAMBRES ACTUELLES (%d)\n", count);
    printf("======================================================================================\n");
    printf("| ID     | Type              | Prix/Nuit | Statut     \n");
    printf("--------------------------------------------------------------------------------------\n");

    // Kan-loopiw 3la ga3 les chambres w kan-affichiwhom
    for (int i = 0; i < count; i++) {
        printf("| %-7d | %-17s | %-9.2f | %-11s\n",
               rooms[i].id,
               rooms[i].type,
               rooms[i].price,
               rooms[i].status);
    }
    printf("======================================================================================\n");
    
    LOG_INFO("Affichage de %d chambres terminé", count);
}

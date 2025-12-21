#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * ROOM MANAGEMENT MODULE - Moroccan Darija Implementation
 * ============================================================================
 * Hna kan-gérir les chambres b daba, b validation w logging dyal debug.h
 * ============================================================================ */

#define MAX_ROOM_TYPE_LENGTH 50
#define MAX_ROOM_STATUS_LENGTH 20

typedef struct {
    int id;                    // Unique ID dyal l-chambre
    char type[MAX_ROOM_TYPE_LENGTH];  // "Single", "Double", "Suite"
    float price;               // Price par nuit
    char status[MAX_ROOM_STATUS_LENGTH]; // "Available" ola "Occupied"
} Room;

/* Function declarations */
void add_room(void);
void list_rooms(void);
int validate_room_data(const Room *room);
void save_room_to_file(const Room *room);
void load_rooms_from_file(Room *rooms, int *count);

#endif /* ROOM_H */

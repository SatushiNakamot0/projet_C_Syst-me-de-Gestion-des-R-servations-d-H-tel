#include "../include/fichiers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * FILE MANAGEMENT IMPLEMENTATION
 * ============================================================================
 * Basic file I/O for persistence. Uses binary format for efficiency.
 * ============================================================================ */

void sauvegarder_clients(Client *clients, int count) {
    FILE *f = fopen("data/clients.dat", "wb");
    if (!f) {
        /* Try creating directory and file */
        system("mkdir -p data 2>/dev/null");
        f = fopen("data/clients.dat", "wb");
        if (!f) return;
    }
    
    fwrite(&count, sizeof(int), 1, f);
    if (count > 0 && clients) {
        fwrite(clients, sizeof(Client), count, f);
    }
    fclose(f);
}

void charger_clients(Client *clients, int *count) {
    FILE *f = fopen("data/clients.dat", "rb");
    if (!f) {
        *count = 0;
        return;
    }
    
    int file_count;
    if (fread(&file_count, sizeof(int), 1, f) != 1) {
        fclose(f);
        *count = 0;
        return;
    }
    
    if (file_count > MAX_CLIENTS) file_count = MAX_CLIENTS;
    if (file_count > 0 && clients) {
        fread(clients, sizeof(Client), file_count, f);
    }
    
    *count = file_count;
    fclose(f);
}

void sauvegarder_chambres(Chambre *chambres, int count) {
    FILE *f = fopen("data/chambres.dat", "wb");
    if (!f) {
        system("mkdir -p data 2>/dev/null");
        f = fopen("data/chambres.dat", "wb");
        if (!f) return;
    }
    
    fwrite(&count, sizeof(int), 1, f);
    if (count > 0 && chambres) {
        fwrite(chambres, sizeof(Chambre), count, f);
    }
    fclose(f);
}

void charger_chambres(Chambre *chambres, int *count) {
    FILE *f = fopen("data/chambres.dat", "rb");
    if (!f) {
        *count = 0;
        return;
    }
    
    int file_count;
    if (fread(&file_count, sizeof(int), 1, f) != 1) {
        fclose(f);
        *count = 0;
        return;
    }
    
    if (file_count > MAX_CHAMBRES) file_count = MAX_CHAMBRES;
    if (file_count > 0 && chambres) {
        fread(chambres, sizeof(Chambre), file_count, f);
    }
    
    *count = file_count;
    fclose(f);
}

void sauvegarder_reservations(Reservation *reservations, int count) {
    FILE *f = fopen("data/reservations.dat", "wb");
    if (!f) {
        system("mkdir -p data 2>/dev/null");
        f = fopen("data/reservations.dat", "wb");
        if (!f) return;
    }
    
    fwrite(&count, sizeof(int), 1, f);
    if (count > 0 && reservations) {
        fwrite(reservations, sizeof(Reservation), count, f);
    }
    fclose(f);
}

void charger_reservations(Reservation *reservations, int *count) {
    FILE *f = fopen("data/reservations.dat", "rb");
    if (!f) {
        *count = 0;
        return;
    }
    
    int file_count;
    if (fread(&file_count, sizeof(int), 1, f) != 1) {
        fclose(f);
        *count = 0;
        return;
    }
    
    if (file_count > MAX_RESERVATIONS) file_count = MAX_RESERVATIONS;
    if (file_count > 0 && reservations) {
        fread(reservations, sizeof(Reservation), file_count, f);
    }
    
    *count = file_count;
    fclose(f);
}

void sauvegarder_factures(Facture *factures, int count) {
    FILE *f = fopen("data/factures.dat", "wb");
    if (!f) {
        system("mkdir -p data 2>/dev/null");
        f = fopen("data/factures.dat", "wb");
        if (!f) return;
    }
    
    fwrite(&count, sizeof(int), 1, f);
    if (count > 0 && factures) {
        fwrite(factures, sizeof(Facture), count, f);
    }
    fclose(f);
}

void charger_factures(Facture *factures, int *count) {
    FILE *f = fopen("data/factures.dat", "rb");
    if (!f) {
        *count = 0;
        return;
    }
    
    int file_count;
    if (fread(&file_count, sizeof(int), 1, f) != 1) {
        fclose(f);
        *count = 0;
        return;
    }
    
    if (file_count > MAX_FACTURES) file_count = MAX_FACTURES;
    if (file_count > 0 && factures) {
        fread(factures, sizeof(Facture), file_count, f);
    }
    
    *count = file_count;
    fclose(f);
}


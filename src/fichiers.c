#include "../include/fichiers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"

// For directory creation
#if defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

/* ============================================================================
 * FILE MANAGEMENT IMPLEMENTATION
 * ============================================================================
 * Basic file I/O for persistence. Uses binary format for efficiency.
 * ============================================================================ */

/**
 * @brief Creates the 'data' directory if it does not already exist.
 * This is a helper function to ensure the directory for data files is available.
 */
static void create_data_directory_if_needed() {
    MKDIR("data"); // This will attempt to create the directory. It will fail if it already exists, which is fine.
}

void sauvegarder_clients(Client *clients, int count) {
    LOG_INFO("Attempting to save %d clients to file", count);
    
    create_data_directory_if_needed();
    FILE *f = fopen("data/clients.dat", "wb");
    if (!f) {
        LOG_ERROR("Failed to create or open data/clients.dat for writing");
        return;
    }
    
    // Write count
    if (fwrite(&count, sizeof(int), 1, f) != 1) {
        LOG_ERROR("Failed to write client count to file");
        fclose(f);
        return;
    }
    
    // Write data
    if (count > 0 && clients) {
        if (fwrite(clients, sizeof(Client), count, f) != (size_t)count) {
            LOG_ERROR("Failed to write full client data to file");
            // File is likely corrupted at this point.
        }
    }
    
    fclose(f);
    LOG_INFO("Successfully saved %d clients to file", count);
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
    
    if (file_count > MAX_CLIENTS) {
        LOG_ERROR("Data file contains more clients (%d) than MAX_CLIENTS (%d). Truncating.", file_count, MAX_CLIENTS);
        file_count = MAX_CLIENTS;
    }

    if (file_count > 0 && clients) {
        size_t items_read = fread(clients, sizeof(Client), file_count, f);
        if (items_read != (size_t)file_count) {
            LOG_ERROR("Error reading client data: expected %d, but only read %zu.", file_count, items_read);
        }
        *count = items_read; // Only count the clients that were actually read
    } else {
        *count = 0;
    }
    
    fclose(f);
}

void sauvegarder_chambres(Chambre *chambres, int count) {
    create_data_directory_if_needed();
    FILE *f = fopen("data/chambres.dat", "wb");
    if (!f) {
        LOG_ERROR("Failed to create or open data/chambres.dat for writing");
        return;
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
    
    if (file_count > MAX_CHAMBRES) {
        LOG_ERROR("Data file contains more rooms (%d) than MAX_CHAMBRES (%d). Truncating.", file_count, MAX_CHAMBRES);
        file_count = MAX_CHAMBRES;
    }

    if (file_count > 0 && chambres) {
        size_t items_read = fread(chambres, sizeof(Chambre), file_count, f);
        if (items_read != (size_t)file_count) {
            LOG_ERROR("Error reading room data: expected %d, but only read %zu.", file_count, items_read);
        }
        *count = items_read; // Only count what was read
    } else {
        *count = 0;
    }
    
    fclose(f);
}

void sauvegarder_reservations(Reservation *reservations, int count) {
    create_data_directory_if_needed();
    FILE *f = fopen("data/reservations.dat", "wb");
    if (!f) {
        LOG_ERROR("Failed to create or open data/reservations.dat for writing");
        return;
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
    
    if (file_count > MAX_RESERVATIONS) {
        LOG_ERROR("Data file contains more reservations (%d) than MAX_RESERVATIONS (%d). Truncating.", file_count, MAX_RESERVATIONS);
        file_count = MAX_RESERVATIONS;
    }

    if (file_count > 0 && reservations) {
        size_t items_read = fread(reservations, sizeof(Reservation), file_count, f);
        if (items_read != (size_t)file_count) {
            LOG_ERROR("Error reading reservation data: expected %d, but only read %zu.", file_count, items_read);
        }
        *count = items_read; // Only count what was read
    } else {
        *count = 0;
    }
    
    fclose(f);
}

void sauvegarder_factures(Facture *factures, int count) {
    create_data_directory_if_needed();
    FILE *f = fopen("data/factures.dat", "wb");
    if (!f) {
        LOG_ERROR("Failed to create or open data/factures.dat for writing");
        return;
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
    
    if (file_count > MAX_FACTURES) {
        LOG_ERROR("Data file contains more invoices (%d) than MAX_FACTURES (%d). Truncating.", file_count, MAX_FACTURES);
        file_count = MAX_FACTURES;
    }

    if (file_count > 0 && factures) {
        size_t items_read = fread(factures, sizeof(Facture), file_count, f);
        if (items_read != (size_t)file_count) {
            LOG_ERROR("Error reading invoice data: expected %d, but only read %zu.", file_count, items_read);
        }
        *count = items_read; // Only count what was read
    } else {
        *count = 0;
    }
    
    fclose(f);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif
#include "../include/data_init.h"
#include "../include/debug.h"

/* ============================================================================
 * TKHLI9 DYAL DOSSIER DYAL DATA
 * ============================================================================
 * Bach nkhedmo 3la Windows w Linux, nkhel9ou dossier dyal data w files
 * ============================================================================ */

int init_data_directory(void) {
    LOG_INFO("Initializing data directory...");
    
    // Nkhel9ou dossier data (ila kan deja kayn ma 3ndna mochkil)
    MKDIR("data");
    
    // Nkhel9ou les fichiers vides bach ma ikounou errors
    const char* files[] = {
        "data/clients.dat",
        "data/chambres.dat", 
        "data/reservations.dat",
        "data/users.dat"
    };
    
    for (int i = 0; i < 4; i++) {
        FILE *f = fopen(files[i], "ab"); // Append binary - ila kayn ma ighlatch
        if (f) {
            fclose(f);
            LOG_DEBUG("Initialized: %s", files[i]);
        } else {
            LOG_ERROR("Failed to create: %s", files[i]);
            return 0;
        }
    }
    
    LOG_INFO("Data directory ready");
    return 1;
}

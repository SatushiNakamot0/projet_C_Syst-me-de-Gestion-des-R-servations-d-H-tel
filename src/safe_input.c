#include "../include/safe_input.h"
#include "../include/debug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ============================================================================
 * IMPLEMENTATION DYAL INPUT SÉCURISÉ
 * ============================================================================
 * Hna kan-3awno l-input b daba, bach n7iyo les problèmes dyal scanf
 * Kan-st7ilo fgets bach n-protegiw l-buffer mn l-overflow
 * ============================================================================ */

void safe_input_string(const char *prompt, char *buffer, int max_length) {
    // N-checkiw les paramètres bach n7iyo l-bugs
    if (!prompt || !buffer || max_length <= 0) {
        LOG_ERROR("safe_input_string: Paramètres invalides");
        return;
    }
    
    // N-affichiw l-prompt w n-netti l-output
    printf("%s", prompt);
    fflush(stdout);
    
    // Kan-st7ilo fgets bach n-protegiw l-buffer mn l-overflow
    if (fgets(buffer, max_length, stdin) == NULL) {
        LOG_ERROR("safe_input_string: Erreur de lecture stdin");
        buffer[0] = '\0';
        return;
    }
    
    // Hna kan-n7iyo l-newline dyal fgets
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        // Ila kan l-input kbir bzzaf, kan-cleariw l-buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    
    LOG_DEBUG("safe_input_string: Lu '%s'", buffer);
}

int safe_input_int(const char *prompt) {
    char buffer[32];
    int value = 0;
    int success = 0;
    
    // Kan-loopiw bach n-weslo l-input valide
    while (!success) {
        safe_input_string(prompt, buffer, sizeof(buffer));
        
        // Hna kan-n7iyo l-espaces w l-newlines
        char *ptr = buffer;
        while (*ptr && isspace(*ptr)) ptr++;
        
        if (*ptr == '\0') {
            printf("Erreur: Veuillez entrer un nombre.\n");
            continue;
        }
        
        // Kan-convertiw l-string l-int
        char *endptr;
        value = (int)strtol(ptr, &endptr, 10);
        
        if (*endptr != '\0') {
            printf("Erreur: Nombre invalide. Veuillez réessayer.\n");
        } else {
            success = 1;
        }
    }
    
    LOG_DEBUG("safe_input_int: Lu %d", value);
    return value;
}

float safe_input_float(const char *prompt) {
    char buffer[64];
    float value = 0.0f;
    int success = 0;
    
    // Kan-loopiw bach n-weslo l-input valide dyal float
    while (!success) {
        safe_input_string(prompt, buffer, sizeof(buffer));
        
        // Kan-n7iyo l-espaces w l-newlines
        char *ptr = buffer;
        while (*ptr && isspace(*ptr)) ptr++;
        
        if (*ptr == '\0') {
            printf("Erreur: Veuillez entrer un nombre.\n");
            continue;
        }
        
        // Kan-convertiw l-string l-float
        char *endptr;
        value = strtof(ptr, &endptr);
        
        if (*endptr != '\0') {
            printf("Erreur: Nombre invalide. Veuillez réessayer.\n");
        } else {
            success = 1;
        }
    }
    
    LOG_DEBUG("safe_input_float: Lu %.2f", value);
    return value;
}

int safe_input_yes_no(const char *prompt) {
    char buffer[16];
    
    // Kan-loopiw bach n-weslo l-réponse valide (oui/non)
    while (1) {
        safe_input_string(prompt, buffer, sizeof(buffer));
        
        // Kan-convertiw l-input l-minuscule bach n-facilitiw l-comparaison
        for (char *p = buffer; *p; p++) {
            *p = tolower(*p);
        }
        
        // Kan-checkiw les réponses possibles
        if (strcmp(buffer, "o") == 0 || strcmp(buffer, "oui") == 0 || 
            strcmp(buffer, "y") == 0 || strcmp(buffer, "yes") == 0) {
            return 1; // Oui
        } else if (strcmp(buffer, "n") == 0 || strcmp(buffer, "non") == 0 || 
                   strcmp(buffer, "no") == 0) {
            return 0; // Non
        } else {
            printf("Erreur: Veuillez répondre par 'oui'/'non' ou 'y'/'n'.\n");
        }
    }
}

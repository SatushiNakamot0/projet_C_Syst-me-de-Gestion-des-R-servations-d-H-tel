#ifndef SAFE_INPUT_H
#define SAFE_INPUT_H

/* ============================================================================
 * SAFE INPUT FUNCTIONS
 * ============================================================================
 * Fonctions sécurisées pour l'entrée utilisateur - remplacent scanf
 * ============================================================================ */

/* Constantes pour les tailles de buffer */
#define MAX_EMAIL_LENGTH 100
#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 20
#define MAX_TYPE_LENGTH 20
#define MAX_DATE_LENGTH 11  /* DD/MM/YYYY + null */

/* Fonctions d'entrée sécurisées */
void safe_input_string(const char *prompt, char *buffer, int max_length);
int safe_input_int(const char *prompt);
float safe_input_float(const char *prompt);
int safe_input_yes_no(const char *prompt);

#endif /* SAFE_INPUT_H */

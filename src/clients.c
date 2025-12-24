#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/clients.h"
#include "../include/structures.h"
#include "../include/fichiers.h" // For sauvegarder_clients
#include "../include/safe_input.h"
#include "../include/debug.h"

/* ============================================================================
 * CLIENT MANAGEMENT (PURE LOGIC)
 * ============================================================================ */

/* Helper: Find client by ID */
int trouver_client_par_id(const Client clients[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (clients[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Helper: Find client by email */
int trouver_client_par_email(const Client clients[], int count, const char *email) {
    for (int i = 0; i < count; i++) {
        if (strcmp(clients[i].email, email) == 0) {
            return i;
        }
    }
    return -1;
}

/* Helper: Generate next ID */
static int get_next_client_id(const Client clients[], int count) {
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (clients[i].id > max_id) {
            max_id = clients[i].id;
        }
    }
    return max_id + 1;
}

int client_ajouter(Client *clients, int *count, const Client *nouveau_client) {
    if (*count >= MAX_CLIENTS) {
        LOG_ERROR("Erreur: Limite de clients atteinte (%d)", MAX_CLIENTS);
        return -1; // Full
    }

    // Validate email uniqueness
    if (trouver_client_par_email(clients, *count, nouveau_client->email) != -1) {
        LOG_ERROR("Email dupliqué: %s", nouveau_client->email);
        return -2; // Duplicate email
    }

    // Add to array
    Client *dest = &clients[*count];
    *dest = *nouveau_client;
    
    // Generate ID
    dest->id = get_next_client_id(clients, *count);

    (*count)++;

    // Persist
    sauvegarder_clients(clients, *count);
    LOG_INFO("Client ajouté: ID %d %s %s", dest->id, dest->nom, dest->prenom);
    return 0; // Success
}

int client_modifier(Client *clients, int count, const Client *modifie) {
    int index = trouver_client_par_id(clients, count, modifie->id);
    if (index == -1) {
        return -1; // Not found
    }

    // Validate email uniqueness (exclude self)
    int email_match = trouver_client_par_email(clients, count, modifie->email);
    if (email_match != -1 && email_match != index) {
        LOG_ERROR("Email dupliqué lors modification: %s", modifie->email);
        return -2; // Duplicate email
    }

    // Update
    clients[index] = *modifie;

    // Persist
    sauvegarder_clients(clients, count);
    LOG_INFO("Client modifié: ID %d", modifie->id);
    return 0;
}

int client_supprimer(Client *clients, int *count, int id) {
    int index = trouver_client_par_id(clients, *count, id);
    if (index == -1) {
        return -1; // Not found
    }

    // Shift remaining
    for (int i = index; i < (*count) - 1; i++) {
        clients[i] = clients[i + 1];
    }
    (*count)--;

    // Persist
    sauvegarder_clients(clients, *count);
    LOG_INFO("Client supprimé: ID %d", id);
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "../include/debug.h"

#define USERS_FILE "data/users.dat"

int auth_login(const char *username, const char *password, char *role_out)
{
    FILE *f = fopen(USERS_FILE, "rb");
    if (!f) return 0; // Mazal ma kaynin users

    User u;
    while (fread(&u, sizeof(User), 1, f) == 1)
    {
        if (strcmp(u.username, username) == 0 && strcmp(u.password, password) == 0)
        {
            if (role_out) strcpy(role_out, u.role);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int auth_register(const char *username, const char *password, const char *role)
{
    // Nchoufou wach had user deja kayn
    FILE *f = fopen(USERS_FILE, "rb");
    if (f)
    {
        User u;
        while (fread(&u, sizeof(User), 1, f) == 1)
        {
            if (strcmp(u.username, username) == 0)
            {
                fclose(f);
                LOG_ERROR("Registration failed: User '%s' already exists", username);
                return 0; 
            }
        }
        fclose(f);
    }

    // Nzidom user jdid l fichier
    f = fopen(USERS_FILE, "ab"); // Append binary
    if (!f) {
        // Ymken l-directory khawya wala ma kayench
        // 3adi, supposons bli data directory kayna deja
        LOG_ERROR("Failed to open users file for writing");
        return 0;
    }

    User new_user;
    strncpy(new_user.username, username, 49);
    strncpy(new_user.password, password, 49);
    strncpy(new_user.role, role, 19);
    
    // Nta2kadou bli khetmna b null
    new_user.username[49] = '\0';
    new_user.password[49] = '\0';
    new_user.role[19] = '\0';

    fwrite(&new_user, sizeof(User), 1, f);
    fclose(f);
    
    LOG_INFO("Registered new user: %s (%s)", username, role);
    return 1;
}

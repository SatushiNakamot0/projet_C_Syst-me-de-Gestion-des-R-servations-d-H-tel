#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char username[50];
    char password[50];
    char role[20];
} User;

/**
 * Validates user credentials against data/users.dat
 * Returns 1 on success (and populates role_out), 0 on failure.
 */
int auth_login(const char *username, const char *password, char *role_out);

/**
 * Registers a new user to data/users.dat
 * Returns 1 on success, 0 on failure (e.g. user exists).
 */
int auth_register(const char *username, const char *password, const char *role);

#endif /* AUTH_H */

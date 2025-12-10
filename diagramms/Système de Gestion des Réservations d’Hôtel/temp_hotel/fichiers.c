#include "fichiers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

static bool ensure_directory(const char *path) {
#ifdef _WIN32
    if (MKDIR(path) == 0) {
        return true;
    }
    if (errno == EEXIST) {
        return true;
    }
    return false;
#else
    if (MKDIR(path) == 0 || errno == EEXIST) {
        return true;
    }
    return false;
#endif
}

static bool ensure_file(const char *path) {
    FILE *file = fopen(path, "ab+");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

static bool read_all(const char *path, void **buffer, size_t *count, size_t element_size) {
    *buffer = NULL;
    *count = 0;

    FILE *file = fopen(path, "rb");
    if (!file) {
        return false;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return false;
    }

    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return false;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return false;
    }

    if (size == 0) {
        fclose(file);
        return true;
    }

    if ((size_t)size % element_size != 0) {
        fclose(file);
        return false;
    }

    size_t elements = (size_t)size / element_size;
    void *tmp = malloc(elements * element_size);
    if (!tmp) {
        fclose(file);
        return false;
    }

    size_t read = fread(tmp, element_size, elements, file);
    fclose(file);

    if (read != elements) {
        free(tmp);
        return false;
    }

    *buffer = tmp;
    *count = elements;
    return true;
}

static bool write_all(const char *path, const void *buffer, size_t count, size_t element_size) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        return false;
    }

    if (count == 0 || !buffer) {
        fclose(file);
        return true;
    }

    size_t written = fwrite(buffer, element_size, count, file);
    fclose(file);
    return written == count;
}

bool fichiers_initialiser(const char *data_dir) {
    if (!ensure_directory(data_dir)) {
        return false;
    }

    const char *files[] = {
        CLIENTS_FILE,
        CHAMBRES_FILE,
        RESERVATIONS_FILE,
        FACTURES_FILE,
        USERS_FILE
    };

    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i) {
        if (!ensure_file(files[i])) {
            return false;
        }
    }

    return true;
}

bool fichiers_charger_clients(Client **clients, size_t *count) {
    return read_all(CLIENTS_FILE, (void **)clients, count, sizeof(Client));
}

bool fichiers_sauvegarder_clients(const Client *clients, size_t count) {
    return write_all(CLIENTS_FILE, clients, count, sizeof(Client));
}

bool fichiers_charger_chambres(Chambre **chambres, size_t *count) {
    return read_all(CHAMBRES_FILE, (void **)chambres, count, sizeof(Chambre));
}

bool fichiers_sauvegarder_chambres(const Chambre *chambres, size_t count) {
    return write_all(CHAMBRES_FILE, chambres, count, sizeof(Chambre));
}

bool fichiers_charger_reservations(Reservation **reservations, size_t *count) {
    return read_all(RESERVATIONS_FILE, (void **)reservations, count, sizeof(Reservation));
}

bool fichiers_sauvegarder_reservations(const Reservation *reservations, size_t count) {
    return write_all(RESERVATIONS_FILE, reservations, count, sizeof(Reservation));
}

bool fichiers_charger_factures(Facture **factures, size_t *count) {
    return read_all(FACTURES_FILE, (void **)factures, count, sizeof(Facture));
}

bool fichiers_sauvegarder_factures(const Facture *factures, size_t count) {
    return write_all(FACTURES_FILE, factures, count, sizeof(Facture));
}

bool fichiers_charger_utilisateurs(Utilisateur **utilisateurs, size_t *count) {
    return read_all(USERS_FILE, (void **)utilisateurs, count, sizeof(Utilisateur));
}

bool fichiers_sauvegarder_utilisateurs(const Utilisateur *utilisateurs, size_t count) {
    return write_all(USERS_FILE, utilisateurs, count, sizeof(Utilisateur));
}

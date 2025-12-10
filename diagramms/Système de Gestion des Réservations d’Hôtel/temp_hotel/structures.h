#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_ID_LENGTH            32
#define MAX_NAME_LENGTH          64
#define MAX_EMAIL_LENGTH         96
#define MAX_PHONE_LENGTH         32
#define MAX_ADDRESS_LENGTH       128
#define MAX_USERNAME_LENGTH      32
#define MAX_PASSWORD_LENGTH      64
#define MAX_ROOM_TYPE_LENGTH     32
#define MAX_ROOM_DESC_LENGTH     128
#define MAX_NOTES_LENGTH         256
#define MAX_NOTIFICATION_MSG     160
#define MAX_RESERVATION_ID       32
#define MAX_FACTURE_ID           32

#define MAX_TOASTS_DISPLAYED     6

typedef enum {
    ROLE_ADMIN = 0,
    ROLE_RECEPTIONIST,
    ROLE_CLIENT
} UserRole;

typedef enum {
    ROOM_DISPONIBLE = 0,
    ROOM_OCCUPEE,
    ROOM_EN_ATTENTE,
    ROOM_MAINTENANCE
} RoomStatus;

typedef enum {
    RESERVATION_EN_ATTENTE = 0,
    RESERVATION_CONFIRMEE,
    RESERVATION_ANNULEE
} ReservationStatus;

typedef enum {
    NOTIFICATION_SUCCESS = 0,
    NOTIFICATION_ERROR,
    NOTIFICATION_INFO,
    NOTIFICATION_WARNING
} NotificationType;

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    char id[MAX_ID_LENGTH];
    char nom[MAX_NAME_LENGTH];
    char prenom[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char telephone[MAX_PHONE_LENGTH];
    char adresse[MAX_ADDRESS_LENGTH];
    char notes[MAX_NOTES_LENGTH];
} Client;

typedef struct {
    char numero[MAX_ID_LENGTH];
    char type[MAX_ROOM_TYPE_LENGTH];
    char description[MAX_ROOM_DESC_LENGTH];
    double prix_par_nuit;
    RoomStatus statut;
    uint8_t capacite;
} Chambre;

typedef struct {
    char id[MAX_RESERVATION_ID];
    char client_id[MAX_ID_LENGTH];
    char chambre_numero[MAX_ID_LENGTH];
    Date check_in;
    Date check_out;
    ReservationStatus statut;
    uint32_t nb_nuits;
    double prix_nuit;
    double montant_total;
} Reservation;

typedef struct {
    char id[MAX_FACTURE_ID];
    char reservation_id[MAX_RESERVATION_ID];
    Date date_emission;
    double montant_total;
} Facture;

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    UserRole role;
    uint8_t salt[16];
    uint8_t password_hash[32];
    bool actif;
} Utilisateur;

typedef struct {
    NotificationType type;
    char message[MAX_NOTIFICATION_MSG];
    double ttl_seconds;
} Notification;

typedef struct {
    Notification items[MAX_TOASTS_DISPLAYED];
    size_t count;
} NotificationQueue;

typedef struct {
    bool is_authenticated;
    Utilisateur utilisateur;
} Session;

struct nk_context;
struct nk_color;

typedef struct {
    struct nk_context *ctx;
    struct nk_color *palette;
} UIContext;

typedef struct {
    Client *clients;
    size_t clients_count;
    Chambre *chambres;
    size_t chambres_count;
    Reservation *reservations;
    size_t reservations_count;
    Facture *factures;
    size_t factures_count;
    Utilisateur *utilisateurs;
    size_t utilisateurs_count;
} DataRepository;

typedef struct {
    DataRepository repo;
    Session session;
    NotificationQueue notifications;
    UIContext ui;
} AppContext;

#endif // STRUCTURES_H

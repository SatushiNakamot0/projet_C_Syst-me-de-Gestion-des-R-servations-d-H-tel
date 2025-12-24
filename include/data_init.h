#ifndef DATA_INIT_H
#define DATA_INIT_H

/* ============================================================================
 * DATA DIRECTORY INITIALIZATION
 * ============================================================================
 * Auto-creates data directory on startup for cross-platform compatibility
 * ============================================================================ */

/* Initialize data directory structure */
int init_data_directory(void);

/* Check if directory exists */
int directory_exists(const char *path);

/* Create directory (cross-platform) */
int create_directory(const char *path);

#endif /* DATA_INIT_H */

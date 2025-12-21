#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include "../include/data_init.h"
#include "../include/debug.h"

/* ============================================================================
 * DATA DIRECTORY INITIALIZATION IMPLEMENTATION
 * ============================================================================
 * Cross-platform directory creation for data folder
 * ============================================================================ */

int directory_exists(const char *path) {
#ifdef _WIN32
    DWORD attrib = GetFileAttributesA(path);
    return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
#endif
}

int create_directory(const char *path) {
#ifdef _WIN32
    int result = _mkdir(path);
    return (result == 0 || errno == EEXIST);
#else
    int result = mkdir(path, 0755);
    return (result == 0 || errno == EEXIST);
#endif
}

int init_data_directory(void) {
    const char *data_dir = "data";
    
    LOG_INFO("Initializing data directory...");
    
    if (directory_exists(data_dir)) {
        LOG_INFO("Data directory already exists: %s", data_dir);
        return 1;
    }
    
    LOG_INFO("Creating data directory: %s", data_dir);
    
    if (create_directory(data_dir)) {
        LOG_INFO("Data directory created successfully");
        return 1;
    } else {
        LOG_ERROR("Failed to create data directory: %s", data_dir);
        return 0;
    }
}

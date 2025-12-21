/**
 * @file debug.h
 * @brief Debugging and logging utilities for the Hotel Reservation System
 * 
 * Provides a cross-platform logging system with different log levels,
 * file/line information, and timestamp support.
 */

#ifndef HOTEL_DEBUG_H
#define HOTEL_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Platform-specific includes */
#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEPARATOR '\\'
#else
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

/* ANSI Color Codes */
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_GRAY    "\x1b[90m"

/* Log levels */
typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_COUNT
} LogLevel;

/* Log configuration */
typedef struct {
    LogLevel min_level;     /* Minimum log level to output */
    FILE* output;           /* Output stream (default: stderr) */
    const char* log_file;   /* Optional log file path */
    int use_colors;         /* Whether to use ANSI colors */
} LogConfig;

/**
 * @brief Initialize the logging system
 * @param config Configuration for the logger (can be NULL for defaults)
 * @return 0 on success, non-zero on error
 */
int log_init(const LogConfig* config);

/**
 * @brief Shutdown the logging system and free resources
 */
void log_shutdown(void);

/**
 * @brief Set the minimum log level
 * @param level Minimum level to log
 */
void log_set_level(LogLevel level);

/**
 * @brief Get the string representation of a log level
 * @param level Log level
 * @return String representation
 */
const char* log_level_string(LogLevel level);

/**
 * @brief Get the ANSI color code for a log level
 * @param level Log level
 * @return ANSI color code string
 */
const char* log_level_color(LogLevel level);

/**
 * @brief Internal logging function (use the macros below instead)
 */
void log_message(LogLevel level, const char* file, int line, const char* fmt, ...);

/* Logging macros */
#define LOG_TRACE(...) log_message(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_message(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Debug assertion */
#ifdef NDEBUG
    #define ASSERT(expr) ((void)0)
#else
    #define ASSERT(expr) \
        do { \
            if (!(expr)) { \
                LOG_FATAL("Assertion failed: %s (%s:%d)", #expr, __FILE__, __LINE__); \
                abort(); \
            } \
        } while(0)
#endif

/* Helper to get just the filename from a path */
static inline const char* get_filename(const char* path) {
    const char* filename = strrchr(path, PATH_SEPARATOR);
    return filename ? filename + 1 : path;
}

#endif /* HOTEL_DEBUG_H */

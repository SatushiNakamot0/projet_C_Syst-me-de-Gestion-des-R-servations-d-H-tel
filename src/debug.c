/**
 * @file debug.c
 * @brief Implementation of debugging and logging utilities
 */

#include "../include/debug.h"
#include <stdarg.h>
#include <errno.h>

/* Default log configuration */
static struct {
    LogLevel min_level;
    FILE* output;
    FILE* file_output;
    int use_colors;
    int initialized;
} log_config = {
    .min_level = LOG_LEVEL_INFO,
    .output = NULL,
    .file_output = NULL,
    .use_colors = 1,
    .initialized = 0
};

/* Log level strings */
static const char* level_strings[] = {
    [LOG_LEVEL_TRACE] = "TRACE",
    [LOG_LEVEL_DEBUG] = "DEBUG",
    [LOG_LEVEL_INFO]  = "INFO ",
    [LOG_LEVEL_WARN]  = "WARN ",
    [LOG_LEVEL_ERROR] = "ERROR",
    [LOG_LEVEL_FATAL] = "FATAL"
};

/* Log level colors */
static const char* level_colors[] = {
    [LOG_LEVEL_TRACE] = ANSI_COLOR_GRAY,
    [LOG_LEVEL_DEBUG] = ANSI_COLOR_CYAN,
    [LOG_LEVEL_INFO]  = ANSI_COLOR_GREEN,
    [LOG_LEVEL_WARN]  = ANSI_COLOR_YELLOW,
    [LOG_LEVEL_ERROR] = ANSI_COLOR_RED,
    [LOG_LEVEL_FATAL] = ANSI_COLOR_MAGENTA
};

int log_init(const LogConfig* config) {
    if (log_config.initialized) {
        return 0; /* Already initialized */
    }

    /* Set defaults */
    log_config.min_level = LOG_LEVEL_INFO;
    log_config.output = stderr;
    log_config.file_output = NULL;
    log_config.use_colors = 1;
    
    /* Apply user configuration if provided */
    if (config) {
        if (config->min_level >= LOG_LEVEL_TRACE && config->min_level < LOG_LEVEL_COUNT) {
            log_config.min_level = config->min_level;
        }
        
        if (config->output) {
            log_config.output = config->output;
        }
        
        if (config->log_file) {
            log_config.file_output = fopen(config->log_file, "a");
            if (!log_config.file_output) {
                fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
                return -1;
            }
        }
        
        log_config.use_colors = config->use_colors;
    }

    /* Disable colors if output is not a terminal */
    if (log_config.use_colors) {
#ifdef _WIN32
        /* Enable VT100 escape codes on Windows 10+ */
        HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            } else {
                log_config.use_colors = 0;
            }
        }
#else
        if (!isatty(fileno(log_config.output))) {
            log_config.use_colors = 0;
        }
#endif
    }

    log_config.initialized = 1;
    LOG_INFO("Logging initialized at level %s", level_strings[log_config.min_level]);
    return 0;
}

void log_shutdown(void) {
    if (!log_config.initialized) {
        return;
    }

    if (log_config.file_output && log_config.file_output != stderr && log_config.file_output != stdout) {
        fclose(log_config.file_output);
    }

    log_config.initialized = 0;
}

void log_set_level(LogLevel level) {
    if (level >= LOG_LEVEL_TRACE && level < LOG_LEVEL_COUNT) {
        log_config.min_level = level;
    }
}

const char* log_level_string(LogLevel level) {
    if (level >= LOG_LEVEL_TRACE && level < LOG_LEVEL_COUNT) {
        return level_strings[level];
    }
    return "UNKNOWN";
}

const char* log_level_color(LogLevel level) {
    if (level >= LOG_LEVEL_TRACE && level < LOG_LEVEL_COUNT && log_config.use_colors) {
        return level_colors[level];
    }
    return "";
}

void log_message(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (!log_config.initialized || level < log_config.min_level) {
        return;
    }

    time_t t = time(NULL);
    struct tm* lt = localtime(&t);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", lt);

    /* Format the message */
    va_list args;
    char message[1024];
    
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    /* Log to stderr with colors */
    if (log_config.output) {
        if (log_config.use_colors) {
            fprintf(log_config.output, "%s[%s]%s %s%s:%d%s: %s%s%s\n",
                log_level_color(level),
                level_strings[level],
                ANSI_COLOR_RESET,
                ANSI_COLOR_GRAY,
                get_filename(file),
                line,
                ANSI_COLOR_RESET,
                log_level_color(level),
                message,
                ANSI_COLOR_RESET);
        } else {
            fprintf(log_config.output, "[%s] %s %s:%d: %s\n",
                level_strings[level],
                timestamp,
                get_filename(file),
                line,
                message);
        }
        fflush(log_config.output);
    }

    /* Log to file without colors */
    if (log_config.file_output) {
        fprintf(log_config.file_output, "[%s] %s %s:%d: %s\n",
            level_strings[level],
            timestamp,
            get_filename(file),
            line,
            message);
        fflush(log_config.file_output);
    }

    /* Abort on fatal errors */
    if (level == LOG_LEVEL_FATAL) {
        abort();
    }
}

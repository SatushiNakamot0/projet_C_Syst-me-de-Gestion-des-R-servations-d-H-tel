#include <stdio.h>
#include <stdlib.h>
#include "ui/ui.h"
#include "ui/ui_state.h"
#include "ui/ui_login.h"
#include "include/debug.h"
#include "include/data_init.h"

/* ============================================================================
 * MAIN ENTRY POINT
 * ============================================================================
 * Hna kan-initializiw l-TUI system w kan-chghlou l-loop dyal l-application.
 * ============================================================================ */

int main(void)
{
    LogConfig log_cfg;
    log_cfg.min_level = LOG_LEVEL_INFO;
    log_cfg.output = stderr;
    log_cfg.log_file = NULL;
    log_cfg.use_colors = 1;
    (void)log_init(&log_cfg);
    atexit(log_shutdown);

    /* Initialize data directory structure */
    if (!init_data_directory()) {
        LOG_ERROR("Failed to initialize data directory");
        return EXIT_FAILURE;
    }

    /* Kan-creyiw UI context */
    UIContext *ctx = ui_context_create();
    if (!ctx) {
        LOG_ERROR("Failed to create UI context");
        return EXIT_FAILURE;
    }
    
    /* Kan-initializiw l-UI */
    if (!ui_init(ctx)) {
        LOG_ERROR("Failed to initialize UI");
        ui_context_destroy(ctx);
        return EXIT_FAILURE;
    }

    LOG_INFO("Application started");
    
    /* Show Login Screen */
    if (!show_login_screen(ctx)) {
        LOG_INFO("Login cancelled/failed. Exiting.");
        ui_cleanup(ctx);
        ui_context_destroy(ctx);
        return EXIT_SUCCESS;
    }

    /* Kan-chghlou l-loop l-kbir dyal l-app */
    ui_run(ctx);
    
    /* Kan-nqaydo w kan-nsddo kolchi b nddaaf */
    ui_cleanup(ctx);
    ui_context_destroy(ctx);

    LOG_INFO("Application stopped");
    
    return EXIT_SUCCESS;
}

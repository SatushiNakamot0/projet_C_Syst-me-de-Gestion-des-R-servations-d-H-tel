#include <stdio.h>
#include <stdlib.h>
#include "ui/ui.h"
#include "ui/ui_state.h"
#include "ui/ui_login.h"
#include "include/debug.h"
#include "include/data_init.h"

/* ============================================================================
 * POINT DYAL D-DKHAL L-PROGRAM
 * ============================================================================
 * Hna ghadi nbdaw l-TUI system w n7llo l-loop l-kbira dyal l-application.
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

    /* Nbdaw b tkhli9 dossier dyal data */
    if (!init_data_directory()) {
        LOG_ERROR("Failed to initialize data directory");
        return EXIT_FAILURE;
    }

    /* N3mlou UI context bash nkhedmo biha */
    UIContext *ctx = ui_context_create();
    if (!ctx) {
        LOG_ERROR("Failed to create UI context");
        return EXIT_FAILURE;
    }
    
    /* Nbdaw l-UI w kolchi dyalha */
    if (!ui_init(ctx)) {
        LOG_ERROR("Failed to initialize UI");
        ui_context_destroy(ctx);
        return EXIT_FAILURE;
    }

    LOG_INFO("Application started");
    
    /* Nwriw l-page dyal login lwla */
    if (!show_login_screen(ctx)) {
        LOG_INFO("Login cancelled/failed. Exiting.");
        ui_cleanup(ctx);
        ui_context_destroy(ctx);
        return EXIT_SUCCESS;
    }

    /* Bdina l-loop l-kbira li ghadi t3awed kolchi */
    ui_run(ctx);
    
    /* Nsaliw w n7llo kolchi b nda9a */
    ui_cleanup(ctx);
    ui_context_destroy(ctx);

    LOG_INFO("Application stopped");
    
    return EXIT_SUCCESS;
}

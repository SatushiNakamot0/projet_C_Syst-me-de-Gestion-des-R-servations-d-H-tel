#include <stdio.h>
#include <stdlib.h>
#include "ui/ui.h"
#include "ui/ui_state.h"

/* ============================================================================
 * MAIN ENTRY POINT
 * ============================================================================
 * Initializes the TUI system and runs the main application loop.
 * ============================================================================ */

int main(void)
{
    /* Create UI context */
    UIContext *ctx = ui_context_create();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create UI context\n");
        return EXIT_FAILURE;
    }
    
    /* Initialize UI */
    if (!ui_init(ctx)) {
        fprintf(stderr, "Error: Failed to initialize UI\n");
        ui_context_destroy(ctx);
        return EXIT_FAILURE;
    }
    
    /* Run main loop */
    ui_run(ctx);
    
    /* Cleanup */
    ui_cleanup(ctx);
    ui_context_destroy(ctx);
    
    return EXIT_SUCCESS;
}

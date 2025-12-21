#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../../ui/ui.h"
#include "../../ui/ui_state.h"

/* Test suite for UI Components */
TestSuite(ui_components, .init = cr_redirect_stdout);

/* Test case for UI initialization */
Test(ui_components, test_ui_init) {
    UIContext *ctx = ui_context_create();
    cr_assert_not_null(ctx, "Failed to create UI context");
    
    bool init_result = ui_init(ctx);
    cr_assert(init_result, "UI initialization failed");
    
    ui_cleanup(ctx);
    ui_context_destroy(ctx);
}

/* Test case for window resize handling */
Test(ui_components, test_window_resize) {
    UIContext *ctx = ui_context_create();
    cr_assert_not_null(ctx, "Failed to create UI context");
    
    // Simulate window resize
    ui_context_resize(ctx, 50, 100);
    cr_assert_eq(ctx->screen_rows, 50, "Screen rows not updated");
    cr_assert_eq(ctx->screen_cols, 100, "Screen columns not updated");
    
    ui_context_destroy(ctx);
}

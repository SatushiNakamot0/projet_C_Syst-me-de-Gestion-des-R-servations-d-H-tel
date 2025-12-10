#ifndef UI_THEME_H
#define UI_THEME_H

#include "nuklear_config.h"

#define COLOR_PRIMARY      nk_rgb(41, 128, 185)
#define COLOR_SECONDARY    nk_rgb(52, 73, 94)
#define COLOR_SUCCESS      nk_rgb(46, 204, 113)
#define COLOR_WARNING      nk_rgb(241, 196, 15)
#define COLOR_DANGER       nk_rgb(231, 76, 60)
#define COLOR_LIGHT        nk_rgb(236, 240, 241)
#define COLOR_DARK         nk_rgb(44, 62, 80)
#define COLOR_GOLD         nk_rgb(212, 175, 55)

void ui_theme_apply(struct nk_context *ctx);

#endif // UI_THEME_H

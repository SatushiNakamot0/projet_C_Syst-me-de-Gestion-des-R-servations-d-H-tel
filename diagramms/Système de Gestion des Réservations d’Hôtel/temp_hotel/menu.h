#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "structures.h"

bool menu_initialize(AppContext *app);
void menu_shutdown(AppContext *app);
bool menu_run(AppContext *app);

#endif // MENU_H

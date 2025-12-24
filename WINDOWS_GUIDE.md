# Guide Windows l'Hotel Reservation System

## Khouya, hadak howa guide bach t'installi w t'runi l'project f Windows 10/11. Ghadi n'kounou n'sta3mlo MinGW-w64 (GCC) w PDCurses, 7it Ncurses standard ma ykhdamch f Windows.

### Step 1: Installi MSYS2 wala MinGW

- MSYS2 howa environment li y3tik gcc w make, li ghadi n'kounou n'khdmo bihom bach n'compilew C code.
- Downloadi MSYS2 men site officiel: https://www.msys2.org/
- Installih f C:/msys64 (default path).
- Ba3d installation, runi MSYS2 UCRT64 terminal w ktb: `pacman -Syu` bach update.
- Ba3d, installi gcc w make: `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make`

### Step 2: Downloadi PDCurses.dll

- PDCurses howa library li ykhdam f Windows bach Ncurses.
- Downloadi PDCurses.dll men site officiel: https://pdcurses.org/
- 7ot PDCurses.dll f same folder m3a l'.exe (ya3ni f build/ folder ba3d compilation).

### Step 3: Runi l'project

- Double-clicki 3la run_windows.bat.
- Hadak script ghadi ychecki build folder, y'run CMake, y'compile, w y'run l'app.
- Law kan chi error, pause ghadi y'khdar l'window mftou7a bach t'chofi.

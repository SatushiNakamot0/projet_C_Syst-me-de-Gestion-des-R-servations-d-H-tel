# Système de Réservation d'Hôtel

## Introduction
Had l-projet howa système dyal gestion w réservation dyal l’hôtel b TUI (Terminal UI). Kaykhlik t-sayyer:

- l-clients
- l-chambres
- l-réservations
- l-facturation

L-application كتخدم ف terminal w كتستعمل `ncurses` باش t3ti واجهة تفاعلية.

## Installation (Windows)
### 1) MinGW (MSYS2)
Bash t-installi MSYS2 (UCRT64) 7it howa اللي كانستعملو f had l-projet.

- Sir l: https://www.msys2.org/
- Installi MSYS2
- 7ell **MSYS2 UCRT64** terminal w dير:

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ncurses
```

### 2) CMake
- Installi CMake (Windows installer): https://cmake.org/download/
- T2kked blli `cmake` kayban f PATH:

```powershell
cmake --version
```

## Compilation (PowerShell)
Hadi hya nafs l-commands اللي خddamna bihom:

```powershell
Set-Location "C:\Users\yazid\OneDrive\Bureau\hotel_reservation_system"

# Kan-mshiwh l-build l-9dim (ila kayn)
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# Kan-configuriw b MinGW Makefiles
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

# Kan-compiliw
cmake --build build
```

> Ila katl9a chi mochkil dyal `nmake`, rah khassek t2kked blli generator howa `MinGW Makefiles` w blli `build/` mmsou7.

## Usage (كيفاش t-lansi l-app)
Mlli tcompili b nja7:

```powershell
.\build\hotel_app.exe
```

Ila bghiti t-chouf logs b colors w file/line, rah `debug.h` kaykhdem b `LOG_INFO/LOG_ERROR/...`.

# Windows Setup Guide

Quick guide for building and running the Hotel Reservation System on Windows.

## Prerequisites

### Install MSYS2 (Recommended)

1. Download MSYS2 from: https://www.msys2.org/
2. Install to default path: `C:\msys64`
3. Open **MSYS2 UCRT64** terminal
4. Update packages:
   ```bash
   pacman -Syu
   ```
5. Install build tools:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-ncurses
   ```

## Building the Project

Open MSYS2 UCRT64 terminal in the project directory:

```bash
# Clean previous builds
make clean

# Compile
make

# Run the application
./hotel_app.exe
```

## Troubleshooting

**Issue: `make` command not found**
- Make sure you're using **MSYS2 UCRT64** terminal, not regular Windows Command Prompt

**Issue: ncurses not found**
- Reinstall ncurses: `pacman -S mingw-w64-ucrt-x86_64-ncurses`

**Issue: Terminal display issues**
- Use Windows Terminal or MSYS2 native terminal for best results
- Avoid CMD.exe as it has limited Unicode support

## Alternative: Using Git Bash

If you have Git for Windows installed:

1. Install ncurses manually or use pre-built binaries
2. Run `make` from Git Bash terminal
3. Execute `./hotel_app.exe`

---

For detailed project documentation, see [README.md](README.md)

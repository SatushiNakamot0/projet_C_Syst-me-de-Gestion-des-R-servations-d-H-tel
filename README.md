# Complete Windows Setup Guide for Hotel Reservation System

## Overview

This guide provides step-by-step instructions to set up the entire development environment for the Hotel Reservation Management System on Windows. We'll use MSYS2 as the development environment, PDCurses for the ncurses library, CMake for build configuration, and Make for compilation.

## Prerequisites

- Windows 10 or 11 (64-bit)
- Administrator privileges for installation
- Internet connection for downloading packages

## Step 1: Install MSYS2

### Download and Install

1. **Download MSYS2**:

   - Go to https://www.msys2.org/
   - Download the latest installer (msys2-x86_64-\*.exe)
   - Run the installer as Administrator
   - Install to the default location: `C:\msys64`

2. **Initial Setup**:
   - After installation, MSYS2 will open automatically
   - Update the package database and core packages:
     ```bash
     pacman -Syu
     ```
   - You may need to close and reopen MSYS2, then run:
     ```bash
     pacman -Su
     ```

## Step 2: Install Required Development Tools

### Open MSYS2 UCRT64 Terminal

- From the Start menu, open "MSYS2 UCRT64"
- This is the terminal we'll use for development

### Install Core Development Tools

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-cmake
```

### Install PDCurses (ncurses for Windows)

```bash
pacman -S mingw-w64-ucrt-x86_64-pdcurses
```

### Verify Installations

```bash
# Check GCC version
gcc --version

# Check Make version
make --version

# Check CMake version
cmake --version

# Check PDCurses installation
ls /ucrt64/include/pdcurses.h
ls /ucrt64/lib/libpdcurses.dll.a
```

## Step 3: Clone the Repository

### Using Git (Recommended)

```bash
# Navigate to your desired workspace directory
cd /c/Users/YourUsername/Documents  # or wherever you want

# Clone the repository
git clone <repository-url> hotel_reservation_system

# Enter the project directory
cd hotel_reservation_system
```

### Alternative: Download ZIP

1. Go to the GitHub repository
2. Click "Code" → "Download ZIP"
3. Extract to your desired location
4. Open MSYS2 UCRT64 terminal and navigate to the extracted folder

## Step 4: Build the Project

### Option A: Automated Setup (Recommended)

```bash
# Run the automated setup script
./setup.bat
```

This script will:

- Create a build directory
- Configure the project with CMake
- Build the executable
- Run the application

### Option B: Manual CMake Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "MinGW Makefiles" ..

# Build the project
cmake --build .

# Copy PDCurses DLL to build directory
cp /ucrt64/bin/libpdcurses.dll .

# Run the application
./hotel_app.exe
```

### Option C: Using Makefile Directly

```bash
# Build using the provided Makefile
make

# Run the application
./hotel_app.exe
```

## Step 5: Verify the Installation

### Run the Application

```bash
# From the project root or build directory
./hotel_app.exe
```

### Expected Output

- A terminal-based interface should appear
- You should see menus for managing reservations, clients, and rooms
- Use arrow keys and Enter to navigate

## Step 6: Development Workflow

### Making Changes

1. Edit source files in `src/` and `ui/` directories
2. Rebuild the project:
   ```bash
   cd build
   cmake --build .
   ```
3. Test your changes:
   ```bash
   ./hotel_app.exe
   ```

### Cleaning Build Files

```bash
# Clean CMake build
cd build
cmake --build . --target clean

# Or use the provided script
../clean.sh
```

## Step 7: Troubleshooting

### Common Issues and Solutions

#### 1. "mingw32-make: Makefile: No such file or directory"

**Problem**: CMake configuration wasn't run first.
**Solution**:

```bash
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

#### 2. "fatal error: pdcurses.h: No such file or directory"

**Problem**: PDCurses headers not found.
**Solution**:

```bash
pacman -S mingw-w64-ucrt-x86_64-pdcurses
```

#### 3. "libpdcurses.dll not found"

**Problem**: PDCurses DLL not in the executable directory.
**Solution**:

```bash
cp /ucrt64/bin/libpdcurses.dll build/
```

#### 4. "Permission denied" when running executable

**Problem**: File permissions issue.
**Solution**:

```bash
chmod +x hotel_app.exe
```

#### 5. CMake can't find PDCurses

**Problem**: CMake configuration fails.
**Solution**: Ensure you're using the correct CMakeLists.txt (the updated one with Windows support).

#### 6. MSYS2 commands not found

**Problem**: PATH not set correctly.
**Solution**: Use MSYS2 UCRT64 terminal, not Command Prompt or PowerShell.

### Environment Variables

Make sure your PATH includes:

- `C:\msys64\ucrt64\bin`
- `C:\msys64\usr\bin`

### Checking Versions

```bash
# Verify all tools are accessible
which gcc
which make
which cmake
pkg-config --list-all | grep pdcurses
```

## Step 8: Project Structure Understanding

```
hotel_reservation_system/
├── src/                    # Main source code
│   ├── main.c             # Application entry point
│   ├── clients.c          # Client management
│   ├── chambres.c         # Room management
│   ├── reservations.c     # Reservation handling
│   ├── facturation.c      # Billing system
│   └── ui.c               # User interface logic
├── include/               # Header files
├── ui/                    # UI components
├── data/                  # Data files (created at runtime)
├── CMakeLists.txt         # CMake configuration
├── Makefile              # Alternative build system
├── setup.bat             # Windows automated setup
├── run_windows.bat       # Quick run script
└── build/                # Build artifacts (created)
```

## Step 9: Advanced Configuration

### Using Vcpkg (Alternative to MSYS2)

If you prefer vcpkg over MSYS2:

1. **Install vcpkg**:

   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.bat
   ./vcpkg integrate install
   ```

2. **Install PDCurses**:

   ```bash
   ./vcpkg install pdcurses
   ```

3. **Configure CMake**:
   ```bash
   cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
   ```

### Custom Build Options

```bash
# Enable tests (if available)
cmake -DENABLE_TESTS=ON ..

# Use different generator
cmake -G "Unix Makefiles" ..

# Specify custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

## Step 10: Contributing and Development

### Setting up Git

```bash
# Configure Git (replace with your info)
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Create a feature branch
git checkout -b feature/your-feature-name
```

### Code Style

- Use consistent indentation (4 spaces)
- Follow C99 standards
- Add comments for complex logic
- Test changes before committing

### Committing Changes

```bash
# Check status
git status

# Add changes
git add .

# Commit with message
git commit -m "Description of changes"

# Push to remote
git push origin feature/your-feature-name
```

## Step 11: Running Tests (If Available)

### Build with Tests

```bash
cmake -DENABLE_TESTS=ON ..
cmake --build .
ctest  # Run tests
```

## Final Notes

### Performance Tips

- Use `make -j$(nproc)` for parallel compilation
- Keep build directories separate for different configurations
- Use `ccache` for faster recompilation (if installed)

### Backup Important Data

- The `data/` directory contains runtime data
- Back up `.dat` files before major changes
- Consider using Git for data versioning if needed

### Getting Help

- Check the README.md for project-specific information
- Review WINDOWS_GUIDE.md for Windows-specific tips
- Search GitHub issues for similar problems
- Ask in project discussions or create an issue

### System Requirements Summary

- **OS**: Windows 10/11 64-bit
- **RAM**: 2GB minimum, 4GB recommended
- **Disk Space**: 2GB free space
- **Tools**: MSYS2, GCC, CMake, Make, PDCurses

This guide covers everything needed to set up, build, and run the Hotel Reservation System on Windows. If you encounter any issues not covered here, please check the troubleshooting section or create an issue in the repository.

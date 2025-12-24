# Hotel Reservation System

A professional Terminal User Interface (TUI) application for managing hotel operations, built with C and ncurses.

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Build](https://img.shields.io/badge/build-make-green.svg)
![License](https://img.shields.io/badge/license-MIT-yellow.svg)

## 📋 Overview

This Hotel Reservation System is a comprehensive terminal-based application designed for managing hotel operations including client management, room inventory, reservations, and billing. Built with ncurses for a rich text-based interface, it features role-based access control and a wizard-style user experience.

## ✨ Features

### Core Functionality
- **Client Management**: Add, edit, search, and manage hotel clients
- **Room Inventory**: Manage room types, pricing, and availability
- **Reservation System**: Create, edit, and cancel reservations with date validation
- **Billing & Invoices**: Automated billing with accurate date calculation and invoice generation

### Advanced Features
- **Role-Based Access Control (RBAC)**:
  - **Admin**: Full system access including user management
  - **Receptionist**: Manage clients, rooms, reservations, and billing
  - **Client**: Self-service portal to view and manage own reservations
  
- **User Authentication**: File-based user database with secure login
- **Client Portal**: Dedicated interface for clients to book rooms and view their reservations
- **Smart Date Handling**: Precise night calculation using `mktime` and `difftime`
- **Invoice Export**: Generate and save invoices to text files
- **Soft Delete**: Reservation cancellation with status tracking (ACTIVE/ANNULEE)

### User Experience
- Beautiful TUI with color themes and visual feedback
- Wizard-style forms with field navigation
- Real-time validation and availability checking
- Intuitive keyboard shortcuts for all operations

## 🚀 Getting Started

### Prerequisites
- GCC compiler
- ncurses library (ncursesw)
- Make

**Windows (MSYS2/MinGW):**
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ncurses
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install build-essential libncurses-dev
```

**macOS:**
```bash
brew install ncurses
```

### Building the Project

```bash
# Clean previous builds
make clean

# Compile the application
make

# Run the application
./hotel_app.exe  # Windows
./hotel_app      # Linux/macOS
```

## 📁 Project Structure

```
hotel_reservation_system/
├── src/              # Business logic and core functionality
│   ├── clients.c     # Client management
│   ├── chambres.c    # Room management
│   ├── reservations.c # Reservation logic
│   ├── facturation.c # Billing calculations
│   ├── auth.c        # User authentication
│   └── fichiers.c    # Data persistence
├── ui/               # User interface components
│   ├── ui.c          # Main UI loop and state machine
│   ├── ui_draw.c     # Screen rendering
│   ├── ui_theme.c    # Color schemes and styling
│   ├── ui_login.c    # Login and registration screens
│   ├── ui_client_portal.c # Client self-service interface
│   └── ...
├── include/          # Header files
├── data/             # Binary data storage (*.dat files)
└── Makefile          # Build configuration
```

## 🎯 Usage

### First Launch
1. On first run, use the default credentials: `admin` / `admin123`
2. Or press `F2` to register a new user

### Navigation
- **Arrow Keys**: Navigate menus and lists
- **Enter**: Select/Confirm
- **ESC**: Back/Cancel
- **Tab**: Move between form fields

### Key Operations

#### Admin/Receptionist
- **[A]dd**: Create new clients, rooms, or reservations
- **[E]dit**: Modify existing records
- **[D]elete**: Remove clients or rooms
- **[S]earch**: Find clients
- **[C]ancel**: Cancel a reservation
- **[B]illing**: Generate invoice for a reservation
- **[P]rint**: Export invoice to file

#### Client Portal
- **My Dashboard**: View reservation summary
- **Book Room**: Create new reservation
- **My Reservations**: View and manage your bookings

## 🔐 Security

- Password masking during input
- Role-based access control
- File-based user database with role assignment
- Separation of client data by user authentication

## 💾 Data Persistence

All data is stored in binary format (`.dat` files) in the `data/` directory:
- `clients.dat`: Client records
- `chambres.dat`: Room inventory
- `reservations.dat`: Reservation records
- `factures.dat`: Invoice history
- `users.dat`: User authentication database

## 🛠️ Development

### Clean Build
```bash
make clean && make
```

### Directory Cleanup
```bash
# Remove all build artifacts and generated files
make clean
rm -f facture_*.txt build_log.txt
```

## 📝 License

This project is released under the MIT License.

## 👥 Contributors

Developed as part of a C programming project focusing on systems programming, data structures, and terminal user interfaces.

---

**Built with ❤️ using C and ncurses**

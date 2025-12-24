#!/bin/bash

# clean.sh: Script to clean the Hotel Reservation System repository
# Removes build artifacts, temp files, and enforces project structure

# Function to print colored output
print_green() {
    echo -e "\033[32m$1\033[0m"
}

# Remove build artifacts
echo "Removing build artifacts..."
rm -rf build/          # Entire build directory
rm -rf obj/            # Object files directory
find . -name "*.o" -type f -delete  # All .o files in the project
rm -f hotel_app        # Root executable (if exists)
rm -f a.out            # Common executable name
find . -name "*.exe" -type f -delete  # Any .exe files

# Remove IDE and temp files
echo "Removing IDE and temporary files..."
rm -rf .vscode/        # VS Code settings/history
find . -name "__MACOSX" -type d -exec rm -rf {} +  # Mac temp folders
find . -name ".DS_Store" -type f -delete          # Mac hidden files
find . -name "*~" -type f -delete                 # Emacs backup files
find . -name "*.swp" -type f -delete              # Vim swap files

# Enforce project structure
echo "Enforcing project structure..."

# Move misplaced .h files from src/ to include/
find src/ -name "*.h" -type f -exec mv {} include/ \;

# Move misplaced .c files from root to src/ (except main.c)
for file in *.c; do
    if [[ "$file" != "main.c" ]]; then
        mv "$file" src/
    fi
done

# Final message
print_green "Project Cleaned Successfully"

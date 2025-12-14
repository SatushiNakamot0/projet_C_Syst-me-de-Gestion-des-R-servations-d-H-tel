CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
OBJ_DIR = obj
SRC_DIR = src

# List of source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/clients.c \
       $(SRC_DIR)/chambres.c \
       $(SRC_DIR)/reservations.c \
       $(SRC_DIR)/facturation.c \
       $(SRC_DIR)/menu.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/fichiers.c

# List of object files (auto-generated)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main target
hotel_app: $(OBJS)
	$(CC) $(OBJS) -o hotel_app

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) hotel_app
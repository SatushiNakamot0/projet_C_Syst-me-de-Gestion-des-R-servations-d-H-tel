CC = gcc
CFLAGS = -Iinclude -Iui -IC:/msys64/ucrt64/include/ncurses -IC:/msys64/ucrt64/include -Wall -Wextra -pedantic -std=c99
LDFLAGS = -LC:/msys64/ucrt64/lib -lncursesw
OBJ_DIR = obj
SRC_DIR = src
UI_DIR = ui

# Source files
SRCS = main.c \
       $(SRC_DIR)/clients.c \
       $(SRC_DIR)/chambres.c \
       $(SRC_DIR)/facturation.c \
       $(SRC_DIR)/reservations.c \
       $(SRC_DIR)/fichiers.c \
       $(SRC_DIR)/data_init.c \
       $(SRC_DIR)/debug.c \
       $(SRC_DIR)/safe_input.c \
       $(SRC_DIR)/auth.c \
       $(UI_DIR)/ui.c \
       $(UI_DIR)/ui_theme.c \
       $(UI_DIR)/ui_layout.c \
       $(UI_DIR)/ui_utils.c \
       $(UI_DIR)/ui_draw.c \
       $(UI_DIR)/ui_input.c \
       $(UI_DIR)/ui_components.c \
       $(UI_DIR)/ui_redraw.c \
       $(UI_DIR)/ui_form.c \
       $(UI_DIR)/ui_reservations.c \
       $(UI_DIR)/ui_clients.c \
       $(UI_DIR)/ui_rooms.c \
       $(UI_DIR)/ui_login.c

# Object files
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Main target
hotel_app: $(OBJS)
	@echo "Linking hotel_app..."
	$(CC) $(OBJS) $(LDFLAGS) -o hotel_app
	@echo "Build complete!"

# Compile source files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR) hotel_app
	@echo "Clean complete!"

# Phony targets
.PHONY: clean all

# Default target
all: hotel_app
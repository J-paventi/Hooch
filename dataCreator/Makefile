# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -ggdb -I$(INC_DIR)

# Executable name
TARGET = $(BIN_DIR)/dataCreator

# Source files
SRC_FILES = $(SRC_DIR)/dataCreator.c \

# Object files (replacing src/ with obj/ and .c with .o)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $(TARGET)

# Compiling object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Include dependencies
$(OBJ_DIR)/dataCreator.o: $(INC_DIR)/dataStructures.h

# Clean
clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/dataCreator

# Phony targets
.PHONY: all clean

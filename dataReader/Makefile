# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -ggdb -I$(INC_DIR)

# Executable name
TARGET = $(BIN_DIR)/dataReader

# Source files
SRC_FILES = $(SRC_DIR)/dataReader.c \
            $(SRC_DIR)/logger.c \
            $(SRC_DIR)/sharedMemory.c \
            $(SRC_DIR)/messageQueue.c \
            $(SRC_DIR)/dcManager.c

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
$(OBJ_DIR)/dataReader.o: $(INC_DIR)/dataStructures.h $(INC_DIR)/logger.h $(INC_DIR)/sharedMemory.h $(INC_DIR)/messageQueue.h $(INC_DIR)/dcManager.h
$(OBJ_DIR)/logger.o: $(INC_DIR)/logger.h $(INC_DIR)/dataStructures.h
$(OBJ_DIR)/sharedMemory.o: $(INC_DIR)/sharedMemory.h $(INC_DIR)/logger.h $(INC_DIR)/dataStructures.h
$(OBJ_DIR)/messageQueue.o: $(INC_DIR)/messageQueue.h $(INC_DIR)/logger.h $(INC_DIR)/dataStructures.h
$(OBJ_DIR)/dcManager.o: $(INC_DIR)/dcManager.h $(INC_DIR)/logger.h $(INC_DIR)/sharedMemory.h $(INC_DIR)/messageQueue.h $(INC_DIR)/dataStructures.h

# Clean
clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/dataReader

# Phony targets
.PHONY: all clean
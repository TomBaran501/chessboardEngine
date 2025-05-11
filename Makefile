# === VARIABLES ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11

SRC_DIR := src
INCLUDE_DIR := lib
BUILD_DIR := build

EXEC := chess
UI_EXEC := build-ui

# === INCLUDES ===
INCLUDE_PATHS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDES := $(foreach dir,$(INCLUDE_PATHS),-I$(dir))

# === SOURCES ET OBJETS ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.c" ! -path "$(SRC_DIR)/ui/*")
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

UI_SRC := $(SRC_DIR)/ui/rendering.c
UI_OBJ := $(BUILD_DIR)/ui/rendering.o

# === BUILD ===
all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	@echo "🔗 Linkage (main)..."
	$(CC) $(OBJ_FILES) -o $@

$(UI_EXEC): $(OBJ_FILES) $(UI_OBJ)
	@echo "🔗 Linkage (UI)..."
	$(CC) $(OBJ_FILES) $(UI_OBJ) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🛠️  Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# === COMMANDES ===
run: $(EXEC)
	@echo "🚀 Running $(EXEC)..."
	./$(EXEC)

ui: $(UI_EXEC)
	@echo "🖼️  Running UI..."
	./$(UI_EXEC)

clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR) $(EXEC) $(UI_EXEC)

re: clean all

.PHONY: all clean re run ui

# === VARIABLES ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11

SRC_DIR := src
UI_DIR := src/ui
INCLUDE_DIR := lib
BUILD_DIR := build
EXEC := chess
UI_EXEC := chess-ui

LDLIBS := `sdl2-config --libs` -lSDL2_image

# === SOURCES ===
SRC_FILES := $(filter-out $(UI_DIR)/rendering.c, $(shell find $(SRC_DIR) -name "*.c"))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

UI_SRC := $(filter-out $(SRC_DIR)/main.c, $(shell find $(SRC_DIR) -name "*.c"))
UI_OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(UI_SRC))

# === INCLUDES ===
INCLUDE_PATHS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDES := $(foreach dir,$(INCLUDE_PATHS),-I$(dir))

# === RÈGLES ===
all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	@echo "🔗 Linkage (main)..."
	$(CC) $(OBJ_FILES) -o $@

$(UI_EXEC): $(UI_OBJ)
	@echo "🔗 Linkage (UI)..."
	$(CC) $(UI_OBJ) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🛠️  Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# === EXÉCUTIONS ===
run: $(EXEC)
	@echo "🚀 Running $(EXEC)..."
	./$(EXEC)

ui: $(UI_EXEC)
	@echo "🖼️  Running $(UI_EXEC)..."
	./$(UI_EXEC)

# === NETTOYAGE ===
clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR) $(EXEC) $(UI_EXEC)

re: clean all

.PHONY: all clean re run ui

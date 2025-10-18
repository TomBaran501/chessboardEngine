# === VARIABLES ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11

SRC_DIR := src
INCLUDE_DIR := lib
BUILD_DIR := build

EXEC := $(BUILD_DIR)/chess
TEST_EXEC := $(BUILD_DIR)/test_runner

LDLIBS := `sdl2-config --libs` -lSDL2_image

# === SOURCES ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")
SRC_FILES := $(filter-out $(SRC_DIR)/main_ai.c, $(SRC_FILES))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# === INCLUDES ===
INCLUDE_PATHS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDES := $(foreach dir,$(INCLUDE_PATHS),-I$(dir))

# === RÈGLES ===
all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	@echo "🔗 Linkage (main)..."
	$(CC) $(OBJ_FILES) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🛠️  Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# === EXÉCUTIONS ===
run: $(EXEC)
	@echo "🚀 Running $(EXEC)..."
	./$(EXEC)


# === NETTOYAGE ===
clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re run ui test

# === TESTS ===
TEST_DIR := tests
TEST_SRC := $(shell find $(TEST_DIR) -name "*.c")

# On prend toutes les sources dans src/chessboard (et sous-dossiers) sauf main.c
SRC_CHESSBOARD := $(shell find $(SRC_DIR)/chessboard -name "*.c")
OBJ_CHESSBOARD := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_CHESSBOARD))

test: $(OBJ_CHESSBOARD) $(TEST_SRC)
	@mkdir -p $(BUILD_DIR)
	@echo "🧪 Compilation des tests..."
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(TEST_EXEC) -lcriterion
	@echo "🚀 Lancement des tests..."
	ulimit -s unlimited && ./$(TEST_EXEC) --jobs 1


# === VARIABLES ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

SRC_DIR := src
INCLUDE_DIR := lib
BUILD_DIR := build
BOT_DIR := bots
TEST_DIR := tests

EXEC := $(BUILD_DIR)/chess
BOT_EXEC := $(BOT_DIR)/bot_v0
TEST_EXEC := $(BUILD_DIR)/test_runner

LDLIBS := `sdl2-config --libs` -lSDL2_image

# === INCLUDES ===
INCLUDE_PATHS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDES := $(foreach dir,$(INCLUDE_PATHS),-I$(dir))

# === SOURCES ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")
SRC_FILES := $(filter-out $(SRC_DIR)/api_bot.c, $(SRC_FILES))
OBJ_FILES := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRC_FILES:.c=.o))

BOT_SRC_FILES := $(shell find $(SRC_DIR)/ai $(SRC_DIR)/chessboard -name "*.c") $(SRC_DIR)/api_bot.c
BOT_OBJ_FILES := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(BOT_SRC_FILES:.c=.o))

TEST_SRC_FILES := $(shell find $(TEST_DIR) -name "*.c")
SRC_CHESSBOARD := $(shell find $(SRC_DIR)/chessboard -name "*.c")
SRC_API := $(shell find $(SRC_DIR)/api -name "*.c")
TEST_OBJ_FILES := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRC_CHESSBOARD:.c=.o)) \
                  $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRC_API:.c=.o))

# === RÈGLES ===
all: $(EXEC)

# --- main executable ---
$(EXEC): $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	@echo "🔗 Linking main executable..."
	$(CC) $(OBJ_FILES) -o $@ $(LDLIBS)

# --- bot executable ---
$(BOT_EXEC): $(BOT_OBJ_FILES)
	@mkdir -p $(BOT_DIR)
	@echo "🔗 Linking bot executable..."
	$(CC) $(BOT_OBJ_FILES) -o $@

# Règle générique pour compiler tous les .c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🛠️  Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# === EXÉCUTIONS ===
run: $(EXEC)
	@echo "🚀 Running $(EXEC)..."
	./$(EXEC)

bot: $(BOT_EXEC)
	@echo "🚀 Running $(BOT_EXEC)..."
	./$(BOT_EXEC)

# === TESTS ===
test: $(TEST_OBJ_FILES) $(TEST_SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	@echo "🧪 Compiling tests..."
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_OBJ_FILES) $(TEST_SRC_FILES) -o $(TEST_EXEC) -lcriterion
	@echo "🚀 Running tests..."
	ulimit -s unlimited && ./$(TEST_EXEC) --jobs 1

# === NETTOYAGE ===
clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re run bot test

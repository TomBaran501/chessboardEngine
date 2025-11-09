# === VARIABLES ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

SRC_DIR := src
INCLUDE_DIR := lib
BUILD_DIR := build

EXEC := $(BUILD_DIR)/chess
BOT_EXEC := $(BUILD_DIR)/bot
TEST_EXEC := $(BUILD_DIR)/test_runner

LDLIBS := `sdl2-config --libs` -lSDL2_image

# === SOURCES ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")
SRC_FILES := $(filter-out $(SRC_DIR)/api_bot.c, $(SRC_FILES))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

BOT_SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")
BOT_SRC_FILES := $(filter-out $(SRC_DIR)/main.c, $(BOT_SRC_FILES))
BOT_OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(BOT_SRC_FILES))

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

BOT_EXEC := bots/bot_v0
BOT_DIR := bots

$(BOT_EXEC): $(BOT_OBJ_FILES)
	@mkdir -p $(BOT_DIR)
	@echo "🔗 Linking bot executable..."
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDLIBS)


# === EXÉCUTIONS ===
run: $(EXEC)
	@echo "🚀 Running $(EXEC)..."
	./$(EXEC)


bot: $(BOT_EXEC)
	@echo "🚀 Running $(BOT_EXEC)..."
	./$(BOT_EXEC)


# === NETTOYAGE ===
clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re run ui test

# === TESTS ===
TEST_DIR := tests
TEST_SRC := $(shell find $(TEST_DIR) -name "*.c")

SRC_CHESSBOARD := $(shell find $(SRC_DIR)/chessboard -name "*.c")
SRC_API := $(shell find $(SRC_DIR)/api -name "*.c")

TEST_SRC_FILES := $(SRC_CHESSBOARD) $(SRC_API)
TEST_OBJ_FILES := $(TEST_SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

test: $(TEST_OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	@echo "🧪 Compilation des tests..."
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_OBJ_FILES) $(TEST_SRC) -o $(TEST_EXEC) -lcriterion
	@echo "🚀 Lancement des tests..."
	ulimit -s unlimited && ./$(TEST_EXEC) --jobs 1



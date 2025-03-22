# Définition des variables
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = build
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
EXEC = mon_programme

# Règle principale : compilation de l'exécutable
all: $(EXEC)

# Compilation de l'exécutable
$(EXEC): $(OBJ_FILES)
	$(CC) $^ -o $@

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier build s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage des fichiers compilés
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Nettoyage complet + dépendances
mrproper: clean
	rm -rf $(EXEC)

# Exécution du programme
run: $(EXEC)
	./$(EXEC)

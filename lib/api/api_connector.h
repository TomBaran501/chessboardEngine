#ifndef API_CONNECTOR_H
#define API_CONNECTOR_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/wait.h>

#define MOVE_SIZE 10
#define FEN_SIZE 256
// Taille maximale pour un message de log renvoyé par le bot
#define LOG_SIZE 1125

typedef struct
{
    FILE *bot_in;
    FILE *bot_out;
    pid_t pid;
} BotConnector;

// Initialise la connexion avec le bot exécutable
int bot_connect(BotConnector *bot, const char *bot_path);

int is_bot_connected(BotConnector *bot);

int bot_set_fen(BotConnector *bot, const char *fen);

// Demande au bot le meilleur coup pour un FEN et récupère aussi un message de log éventuel
// - `move_str` doit avoir la taille MOVE_SIZE
// - `log_msg` doit avoir la taille LOG_SIZE (peut être vide si aucun message)
// - `timeout_sec` délai (en secondes) pour la lecture du coup principal
int bot_get_best_move(BotConnector *bot, char *move_str, char *log_msg, int timeout_sec);

// Envoie un coup au bot pour mise à jour de son état interne
int bot_play_move(BotConnector *bot, const char *move_str);

// Ferme proprement la connexion avec le bot
void bot_disconnect(BotConnector *bot);

#endif

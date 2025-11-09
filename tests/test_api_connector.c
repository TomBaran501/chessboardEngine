#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "api/api_connector.h"
#include <sys/stat.h>

const char *bot_path = "./bots/bot_v0";

// === Tests de base ===

Test(api_connector, connect_and_get_move)
{
    printf("TEST BOT API (STARTPOS)... \n");
    BotConnector bot;
    char best_move[6];

    int res = bot_connect(&bot, bot_path);
    cr_assert_eq(res, 0, "Échec de la connexion au bot");

    // Demande du meilleur coup
    res = bot_get_best_move(&bot, best_move);
    cr_assert_eq(res, 0, "bot_get_best_move a échoué");
    cr_assert_neq(best_move[0], '\0', "Le bot n’a pas renvoyé de coup");

    // Joue le coup
    res = bot_play_move(&bot, best_move);
    cr_assert_eq(res, 0, "bot_play_move a échoué");

    bot_disconnect(&bot);
    printf("TEST BOT API (STARTPOS) OK ✅\n");
}

// === Test de reconnexion ===
Test(api_connector, reconnect_after_disconnect)
{
    BotConnector bot;
    char best_move[MOVE_SIZE];

    int res = bot_connect(&bot, bot_path);
    cr_assert_eq(res, 0);

    bot_disconnect(&bot);

    res = bot_connect(&bot, bot_path);
    cr_assert_eq(res, 0);

    res = bot_get_best_move(&bot, best_move);
    cr_assert_eq(res, 0);
    cr_assert_neq(best_move[0], '\0', "Le bot n’a pas renvoyé de coup");

    bot_disconnect(&bot);
    printf("TEST BOT API reconnect_after_disconnect OK ✅\n");
}


Test(api_connector, get_move_midgame)
{
    BotConnector bot;
    const char *fen = "8/5P2/8/3k4/8/3K4/8/8 w - - 0 1";
    const char *move = "f7f8q";

    int res = bot_connect(&bot, bot_path);
    cr_assert_eq(res, 0, "Échec de la connexion au bot");

    bot_set_fen(&bot, fen);
    res = bot_play_move(&bot, move);
    cr_assert_eq(res, 0, "bot play move (promotion) à échoué");

    bot_disconnect(&bot);
    printf("TEST BOT API Fen Code OK ✅\n");
}
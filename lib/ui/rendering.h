#include "variables.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"
#include "ai/ai.h"
#include "ai/evaluation.h"
#include "api_connector.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

enum
{
    HvH = -1,
    AvA = 2,
}; // pour l'attribut ia_color

typedef struct
{
    SDL_Event *event;
    Chessboard *board;
    SDL_Renderer *renderer;
    GenericList *colored_squares;
    BotConnector *bot1;
    BotConnector *bot2;
    int *GameState;
} GameEnvironement;

#pragma once

int ui_main_loop(char *startpos, int color_ai);

int ui_bot_match(char *bot1_path, char *bot2_path, int nb_matches);
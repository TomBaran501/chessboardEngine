#include "variables.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"
#include "ai/ai.h"
#include "ai/evaluation.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

enum
{
    HvH = -1,
    AvA = 2,
}; // pour l'attribut ia_color 

#pragma once
void draw_board(SDL_Renderer *renderer);

int ui_game_loop(char *startpos, int color_ai);

static void reset_colored_squares(GenericList *colored_squares);

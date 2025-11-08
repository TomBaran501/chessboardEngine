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

int ui_main_loop(char *startpos, int color_ai);

int handle_SDL_events(SDL_Event *event,
                              Chessboard *board,
                              int color_ai,
                              SDL_Renderer *renderer,
                              GenericList *colored_squares,
                              int *clicked_square,
                              int *nbmoves);

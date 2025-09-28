#include "variables.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"
#include "ai/ai.h"
#include "ai/evaluation.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#pragma once
void draw_board(SDL_Renderer *renderer);

int ui_game_loop(char *startpos, int color_ai);

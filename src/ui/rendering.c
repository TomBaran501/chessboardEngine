#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "chessboard.h"

#define TILE_SIZE 80
#define BOARD_SIZE 8

void render_board(SDL_Renderer *renderer, SDL_Texture *textures[12], const char *fen) {
    int x = 0, y = 0;
    const char *ptr = fen;

    while (*ptr && *ptr != ' ') {
        if (*ptr == '/') {
            y++;
            x = 0;
        } else if (*ptr >= '1' && *ptr <= '8') {
            x += *ptr - '0';
        } else {
            SDL_Rect dest = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            const char *filename = piece_to_filename(*ptr);

            if (filename) {
                for (int i = 0; i < 12; ++i) {
                    if (textures[i] && strcmp(filename, piece_to_filename("PNBRQKpnbrqk"[i])) == 0) {
                        SDL_RenderCopy(renderer, textures[i], NULL, &dest);
                        break;
                    }
                }
            }
            x++;
        }
        ptr++;
    }
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Chess UI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          TILE_SIZE * 8, TILE_SIZE * 8, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *textures[12];
    const char *order = "PNBRQKpnbrqk";
    for (int i = 0; i < 12; i++) {
        SDL_Surface *surface = IMG_Load(piece_to_filename(order[i]));
        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    Chessboard board;
    init_chessboard_from_fen(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    const char *fen = renvoyer_fen_code(&board);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        // Draw background
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                SDL_Rect tile = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_SetRenderDrawColor(renderer, (x + y) % 2 == 0 ? 240 : 100, 
                                                    (x + y) % 2 == 0 ? 217 : 100,
                                                    (x + y) % 2 == 0 ? 181 : 100, 255);
                SDL_RenderFillRect(renderer, &tile);
            }
        }

        render_board(renderer, textures, fen);
        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 12; i++) SDL_DestroyTexture(textures[i]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

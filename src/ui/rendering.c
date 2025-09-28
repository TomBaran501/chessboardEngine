#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "rendering.h"
#include "variables.h"
#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"
#include "ai/ai.h"
#include "ai/evaluation.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

SDL_Color BEIGE = {238, 238, 210, 255};      // clair
SDL_Color DARK_BEIGE = {208, 185, 170, 255}; // foncé

SDL_Color DARK_GREEN = {118, 150, 86, 255};  // clair
SDL_Color DARKER_GREEN = {80, 110, 55, 255}; // foncé

SDL_Texture *piece_textures[12];

char *endgame = "8/3K4/4P3/8/8/8/6k1/7q w - - 0 1";
char *start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void draw_board(SDL_Renderer *renderer)
{
    // Dessine les cases
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if ((row + col) % 2 == 1)
                SDL_SetRenderDrawColor(renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);
            else
                SDL_SetRenderDrawColor(renderer, BEIGE.r, BEIGE.g, BEIGE.b, BEIGE.a);

            SDL_Rect square = {
                .x = col * SQUARE_SIZE,
                .y = row * SQUARE_SIZE,
                .w = SQUARE_SIZE,
                .h = SQUARE_SIZE};
            SDL_RenderFillRect(renderer, &square);
        }
    }
}

/// Renvoie l'index de la case cliquée (0 à 63), ou -1 si aucun clic gauche
int get_colored_square(SDL_Event *event)
{
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        int x = event->button.x;
        int y = event->button.y;

        int col = x / SQUARE_SIZE;
        int row = y / SQUARE_SIZE;

        if (col >= 0 && col < 8 && row >= 0 && row < 8)
        {
            return row * 8 + col;
        }
    }
    return -1;
}

SDL_Color swap_square_color(int beige, int colored)
{
    if (beige && colored)
    {
        return BEIGE;
    }
    if (!beige && !colored)
    {
        return DARKER_GREEN;
    }
    if (beige && !colored)
    {
        return DARK_BEIGE;
    }
    else
    {
        return DARK_GREEN;
    }
}

void draw_colored_square(SDL_Renderer *renderer, int square, SDL_Color color)
{
    int file = square % 8; // colonne (0 à 7)
    int rank = square / 8; // ligne

    SDL_Rect rect;
    rect.x = file * SQUARE_SIZE;
    rect.y = rank * SQUARE_SIZE;
    rect.w = SQUARE_SIZE;
    rect.h = SQUARE_SIZE;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void swap_color_squares(GenericList *list_squares, int colored, SDL_Renderer *renderer)
{
    for (int i = 0; i < list_squares->size; i++)
    {
        int square = ((int *)list_squares->data)[i];
        int beige = (square / 8 + square % 8) % 2 == 0 ? 1 : 0;
        draw_colored_square(renderer, square, swap_square_color(beige, colored));
    }
}

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        SDL_Log("Erreur IMG_Load: %s", IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void load_textures(SDL_Renderer *renderer)
{
    piece_textures[PION_B] = load_texture(renderer, "assets/pieces/Pion_B.png");
    piece_textures[PION_N] = load_texture(renderer, "assets/pieces/Pion_N.png");
    piece_textures[CAVAL_B] = load_texture(renderer, "assets/pieces/Cavalier_B.png");
    piece_textures[CAVAL_N] = load_texture(renderer, "assets/pieces/Cavalier_N.png");
    piece_textures[FOU_B] = load_texture(renderer, "assets/pieces/Fou_B.png");
    piece_textures[FOU_N] = load_texture(renderer, "assets/pieces/Fou_N.png");
    piece_textures[REINE_B] = load_texture(renderer, "assets/pieces/Reine_B.png");
    piece_textures[REINE_N] = load_texture(renderer, "assets/pieces/Reine_N.png");
    piece_textures[ROI_B] = load_texture(renderer, "assets/pieces/Roi_B.png");
    piece_textures[ROI_N] = load_texture(renderer, "assets/pieces/Roi_N.png");
    piece_textures[TOUR_B] = load_texture(renderer, "assets/pieces/Tour_B.png");
    piece_textures[TOUR_N] = load_texture(renderer, "assets/pieces/Tour_N.png");
}

void render_fen(SDL_Renderer *renderer, const char *fen)
{
    int fen_to_texture_index[12] = {0};
    fen_to_texture_index['P'] = PION_B;
    fen_to_texture_index['p'] = PION_N;
    fen_to_texture_index['N'] = CAVAL_B;
    fen_to_texture_index['n'] = CAVAL_N;
    fen_to_texture_index['B'] = FOU_B;
    fen_to_texture_index['b'] = FOU_N;
    fen_to_texture_index['Q'] = REINE_B;
    fen_to_texture_index['q'] = REINE_N;
    fen_to_texture_index['K'] = ROI_B;
    fen_to_texture_index['k'] = ROI_N;
    fen_to_texture_index['R'] = TOUR_B;
    fen_to_texture_index['r'] = TOUR_N;

    int row = 0, col = 0;
    for (const char *c = fen; *c && row < 8; ++c)
    {
        if (*c == ' ')
            break; // Fin de la partie plateau du FEN
        if (*c == '/')
        {
            row++;
            col = 0;
        }
        else if (*c >= '1' && *c <= '8')
        {
            col += *c - '0';
        }
        else
        {
            int idx = fen_to_texture_index[(int)*c];
            if (piece_textures[idx])
            {
                SDL_Rect dst = {
                    .x = col * SQUARE_SIZE,
                    .y = row * SQUARE_SIZE,
                    .w = SQUARE_SIZE,
                    .h = SQUARE_SIZE};
                SDL_RenderCopy(renderer, piece_textures[idx], NULL, &dst);
            }
            col++;
        }
    }
}

int set_moves(Chessboard *board, Move moves[250], int pos_piece, GenericList *colored_squares, SDL_Renderer *renderer)
{
    int nbmoves = getlegalmoves(pos_piece, board, moves);
    if (nbmoves == 0)
        return 0;

    for (int i = 0; i < nbmoves; i++)
    {
        int to = moves[i].to;
        if (!is_in_list(colored_squares, &to)) // Pour les coups de promotion
            list_add(colored_squares, &to);
    }
    swap_color_squares(colored_squares, 0, renderer);
    return nbmoves;
}

void render_play_move(Chessboard *board, Move moves[250], int to, int nbmoves)
{
    Move move;
    for (int i = 0; i < nbmoves; i++)
    {
        if (moves[i].to == to)
            move = moves[i];
    }
    play_move(board, move);
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Échiquier",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window)
    {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    load_textures(renderer);

    GenericList *colored_squares = malloc(sizeof(GenericList));
    list_init(colored_squares, sizeof(int));

    char *fen = malloc(100);
    int nbmoves = 0;

    Chessboard board;
    init_chessboard_from_fen(&board, start_pos);
    initialise_ai();
    int clicked_square;

    bool running = true;
    SDL_Event event;
    draw_board(renderer);
    return_fen_code(&board, fen);
    render_fen(renderer, fen);

    while (running)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            int square = get_colored_square(&event);
            if (board.white_to_play)
            {
                if (square != -1)
                {
                    Move list_moves[250];
                    SDL_RenderClear(renderer);
                    draw_board(renderer);

                    if (colored_squares->size == 0)
                    {
                        nbmoves = set_moves(&board, list_moves, square, colored_squares, renderer);
                        clicked_square = square;
                    }

                    else
                    {
                        if (is_in_list(colored_squares, &square))
                        {
                            nbmoves = getlegalmoves(clicked_square, &board, list_moves);
                            render_play_move(&board, list_moves, square, nbmoves);
                        }

                        swap_color_squares(colored_squares, 1, renderer);
                        list_free(colored_squares);
                        list_init(colored_squares, sizeof(int));
                        nbmoves = 0;
                    }
                    return_fen_code(&board, fen);
                    render_fen(renderer, fen);
                }
            }
            else
            {
                Move move = get_best_move(board);
                play_move(&board, move);
                return_fen_code(&board, fen);
                SDL_RenderClear(renderer);
                draw_board(renderer);
                render_fen(renderer, fen);
            }
        }

        SDL_RenderPresent(renderer);
    }

    list_free(colored_squares);
    free(colored_squares);
    free(fen);

    for (int i = 0; i < 12; i++)
    {
        if (piece_textures[i])
            SDL_DestroyTexture(piece_textures[i]);
    }
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

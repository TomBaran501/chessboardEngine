#include <SDL2/SDL.h>
#include <stdbool.h>
#include "rendering.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

SDL_Color BEIGE = {238, 238, 210, 255};      // clair
SDL_Color DARK_BEIGE = {208, 185, 170, 255}; // foncé

SDL_Color DARK_GREEN = {118, 150, 86, 255};  // clair
SDL_Color DARKER_GREEN = {80, 110, 55, 255}; // foncé

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

    SDL_RenderPresent(renderer);
    // SDL_Delay(16);  // Limite à environ 60 FPS
}

/// Renvoie l'index de la case cliquée (0 à 63), ou -1 si aucun clic gauche
int get_clicked_square(SDL_Event *event)
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

SDL_Color swap_square_color(SDL_Color original)
{
    if (original.r == BEIGE.r && original.g == BEIGE.g && original.b == BEIGE.b)
    {
        return DARK_BEIGE;
    }
    else if (original.r == DARK_GREEN.r && original.g == DARK_GREEN.g && original.b == DARK_GREEN.b)
    {
        return DARKER_GREEN;
    }
    else if (original.r == DARK_BEIGE.r && original.g == DARK_BEIGE.g && original.b == DARK_BEIGE.b)
    {
        return BEIGE;
    }
    else if (original.r == DARKER_GREEN.r && original.g == DARKER_GREEN.g && original.b == DARKER_GREEN.b)
    {
        return DARK_GREEN;
    }
    return original; // pas reconnu, inchangé
}

void draw_colored_square(SDL_Renderer *renderer, int square, SDL_Color color)
{
    int file = square % 8; // colonne (0 à 7)
    int rank = square / 8; // ligne inversée (0 en bas, 7 en haut)

    SDL_Rect rect;
    rect.x = file * SQUARE_SIZE;
    rect.y = rank * SQUARE_SIZE;
    rect.w = SQUARE_SIZE;
    rect.h = SQUARE_SIZE;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
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

    bool running = true;
    SDL_Event event;
    draw_board(renderer);
    while (running)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            int square = get_clicked_square(&event);
            if (square != -1)
            {
                printf("Case cliquée : %d (ligne %d, colonne %d)\n", square, square / 8, square % 8);
                SDL_Color original = (square / 8 + square % 8) % 2 == 0 ? BEIGE : DARK_GREEN;
                draw_colored_square(renderer, square, swap_square_color(original));
            }
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include "rendering.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

SDL_Color BEIGE = {238, 238, 210, 255};      // clair
SDL_Color DARK_BEIGE = {208, 185, 170, 255}; // foncé

SDL_Color DARK_GREEN = {118, 150, 86, 255};  // clair
SDL_Color DARKER_GREEN = {80, 110, 55, 255}; // foncé

SDL_Texture *piece_textures[12];

char *endgame = "8/3K4/4P3/8/8/8/6k1/7q w - - 0 1";
char *start_pos = "8/3K4/4P3/8/8/8/6k1/7q w - - 0 1";

static void draw_board(SDL_Renderer *renderer)
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

static inline GameEnvironement create_game_environment(SDL_Event *event, Chessboard *board, SDL_Renderer *renderer, GenericList *colored_squares)
{
    GameEnvironement env;
    env.event = event;
    env.board = board;
    env.renderer = renderer;
    env.colored_squares = colored_squares;
    return env;
}

/// Renvoie l'index de la case cliquée (0 à 63), ou -1 si aucun clic gauche
static int get_colored_square(SDL_Event *event)
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

static SDL_Color swap_square_color(int beige, int colored)
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

static void draw_colored_square(SDL_Renderer *renderer, int square, SDL_Color color)
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

static void swap_color_squares(GenericList *list_squares, SDL_Renderer *renderer)
{
    for (int i = 0; i < list_squares->size; i++)
    {
        int square = ((int *)list_squares->data)[i];
        int beige = (square / 8 + square % 8) % 2 == 0 ? 1 : 0;
        draw_colored_square(renderer, square, swap_square_color(beige, 0));
    }
}

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
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

static void load_textures(SDL_Renderer *renderer)
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

int fen_char_to_texture(char c)
{
    switch (c)
    {
    case 'P':
        return PION_B;
    case 'p':
        return PION_N;
    case 'N':
        return CAVAL_B;
    case 'n':
        return CAVAL_N;
    case 'B':
        return FOU_B;
    case 'b':
        return FOU_N;
    case 'Q':
        return REINE_B;
    case 'q':
        return REINE_N;
    case 'K':
        return ROI_B;
    case 'k':
        return ROI_N;
    case 'R':
        return TOUR_B;
    case 'r':
        return TOUR_N;
    default:
        return -1; // pas de pièce
    }
}

static void render_fen(SDL_Renderer *renderer, const char *fen)
{
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
            int idx = fen_char_to_texture(*c);
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

static int set_moves(Chessboard *board, Move moves[250], int pos_piece, GenericList *colored_squares)
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
    return nbmoves;
}

static void render_play_move(Chessboard *board, Move moves[250], int to, int nbmoves)
{
    Move move;
    for (int i = 0; i < nbmoves; i++)
    {
        if (moves[i].to == to)
            move = moves[i];
    }
    play_move(board, move);
}

static void ui_refresh_board(GameEnvironement env)
{
    char *fen = calloc(400, 1);
    if (!fen)
        return;

    return_fen_code(env.board, fen);

    SDL_RenderClear(env.renderer);
    draw_board(env.renderer);

    if (env.colored_squares->size != 0)
        swap_color_squares(env.colored_squares, env.renderer);

    render_fen(env.renderer, fen);

    SDL_RenderPresent(env.renderer);
    free(fen);
}

// Vérifie qu'il s'agisse bien d'un click sur une case valide
static bool invalid_event(SDL_Event *event, int square)
{
    if (event->type != SDL_MOUSEBUTTONDOWN && event->type != SDL_MOUSEBUTTONUP)
        return true;

    if (square == -1)
        return true;

    return false;
}

// Réinitialiser les cases colorées
static void reset_colored_squares(GenericList *colored_squares)
{
    list_free(colored_squares);
    list_init(colored_squares, sizeof(int));
}

// Gère les mouvements et cliks du joueur
static void handle_player_event(GameEnvironement env, int *clicked_square)
{
    int square = get_colored_square(env.event);
    if (invalid_event(env.event, square))
        return;

    Move list_moves[250];

    SDL_RenderClear(env.renderer);
    draw_board(env.renderer);

    if (env.colored_squares->size == 0)
    {
        set_moves(env.board, list_moves, square, env.colored_squares);
        *clicked_square = square;
    }
    else
    {
        if (is_in_list(env.colored_squares, &square))
        {
            int nbmoves = getlegalmoves(*clicked_square, env.board, list_moves);
            render_play_move(env.board, list_moves, square, nbmoves);
        }
        reset_colored_squares(env.colored_squares);
    }
}

static GameEnvironement init_game_environment(char *startpos, SDL_Renderer *renderer)
{
    GenericList *colored_squares = malloc(sizeof(GenericList));
    list_init(colored_squares, sizeof(int));

    Chessboard *board = malloc(sizeof(Chessboard));
    init_chessboard_from_fen(board, startpos);

    SDL_Event *event = malloc(sizeof(SDL_Event));

    GameEnvironement env = create_game_environment(event, board, renderer, colored_squares);
    return env;
}


// Libère toutes les ressources SDL + internes
static void cleanup_ui(SDL_Renderer *renderer, SDL_Window *window, GenericList *colored_squares, Chessboard *board)
{
    if (colored_squares)
        list_free(colored_squares);

    if (board)
        free(board);

    for (int i = 0; i < 12; i++)
        if (piece_textures[i])
            SDL_DestroyTexture(piece_textures[i]), piece_textures[i] = NULL;

    IMG_Quit();
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

// Raccourci pour initialiser la fenêtre et le renderer SDL
static bool init_sdl(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Échiquier",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window)
    {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer)
    {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

static bool is_human_turn(int color_ai, int color)
{
    return (color_ai == -1) || (color != color_ai && color_ai != 2);
}

static bool is_ai_turn(int color_ai, int color)
{
    return (color_ai == 2 || color == color_ai);
}

static bool handle_SDL_events(int color_ai, int *clicked_square, GameEnvironement env)
{
    if (env.event->type == SDL_QUIT)
        return false;

    int color = env.board->white_to_play ? WHITE : BLACK;

    if (is_human_turn(color_ai, color))
    {
        handle_player_event(env, clicked_square);
        ui_refresh_board(env);
    }
    return true;
}

static void handle_ai_turn(int color_ai, GameEnvironement env)
{
    int color = env.board->white_to_play ? WHITE : BLACK;

    if (is_ai_turn(color_ai, color))
    {
        Move move = get_best_move(*env.board);
        play_move(env.board, move);
        ui_refresh_board(env);
        SDL_Delay(50);
    }

    SDL_Delay(10);
}

static void game_loop(char *startpos, SDL_Renderer *renderer, int color_ai, SDL_Window *window)
{
    GameEnvironement env = init_game_environment(startpos, renderer);
    int clicked_square = -1;
    bool running = true;
    ui_refresh_board(env);

    while (running)
    {
        while (SDL_PollEvent(env.event))
        {
            if (!handle_SDL_events(color_ai, &clicked_square, env))
                running = false;
        }
        handle_ai_turn(color_ai, env);
    }
    cleanup_ui(renderer, window, env.colored_squares, env.board);
}

// --- Boucle principale du jeu ---
int ui_main_loop(char *startpos, int color_ai)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!init_sdl(&window, &renderer))
        return 1;

    load_textures(renderer);

    game_loop(startpos, renderer, color_ai, window);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "p4.h"

//TODO: Use Surface for rendering the board

#define R_W 50
#define R_MARGIN 10


struct p4_column {
    bool hover;
    SDL_Rect c;
};

typedef struct p4_columns {
    struct p4_column* c;
    int nb;
} P4_Columns;

static SDL_Color p4_repr(CASE_COLOR color) {
    SDL_Color c;
    c.a = 255;
    switch (color){
        case NONE: {
            c.r = 84;
            c.g = 84;
            c.b = 84;
        }
        break;
        case RED: {
            c.r = 255;
            c.g = 0;
            c.b = 0;
        }
        break;
        case YELLOW: {
            c.r = 255;
            c.g = 255;
            c.b = 0;
        }
        break;
        default: {
            assert(false);
        }
    }
    return c;
}

static void p4_render_background(SDL_Renderer* ren, int ncol, int nrow) {
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.h = nrow * (R_W + R_MARGIN) + R_MARGIN;
    r.w = ncol * (R_W + R_MARGIN) + R_MARGIN;
    SDL_SetRenderDrawColor(ren, 2, 121, 194, 255);
    SDL_RenderFillRect(ren, &r);
}

static void p4_display_board(SDL_Renderer* ren, P4_Game game) {
    SDL_Rect r;
    r.w = R_W;
    r.h = R_W;
    
    int pos_w = R_MARGIN;
    int pos_h = R_MARGIN;
    bool won = p4_is_won(game);
    
    p4_render_background(ren, game.size.ncol, game.size.nrow);

    for (int j = game.size.nrow - 1; j >= 0; j--) {
        for (int i = 0; i < game.size.ncol; i++) {
            r.x = pos_w;
            r.y = pos_h;
            
            SDL_Color color = p4_repr(game.board[i][j].p);

            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(ren, &r);
            
            if (won && game.board[i][j].winning_move) {
                const int margin = 10;
                SDL_SetRenderDrawColor(ren, 0, 255, 0, 200);
                r.x = r.x + margin;
                r.y = r.y + margin;
                r.w = r.w - margin * 2;
                r.h = r.h - margin * 2;
                
                SDL_RenderFillRect(ren, &r);
                
                r.w = r.w + margin * 2;
                r.h = r.h + margin * 2;
            }

            pos_w = pos_w + R_W + R_MARGIN;
        }
        pos_w = R_MARGIN;
        pos_h = pos_h + R_W + R_MARGIN;
    }
}

static void p4_display_columns(SDL_Renderer* ren, P4_Columns columns) {
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 50);
    for (int i = 0; i < columns.nb; i++) {
        if (columns.c[i].hover) {
            SDL_RenderFillRect(ren, &(columns.c[i].c));            
        }
    }
}

//TODO: prebake the textures
static void render_text(SDL_Renderer* ren, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_Rect r = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ren, texture, NULL, &r);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static void p4_display_game_info(SDL_Renderer* ren, TTF_Font* font, P4_Game game) {
    char text[20];
    const int font_size = 19;
    
    int x = R_MARGIN;
    int y = game.size.nrow * (R_W + R_MARGIN) + R_MARGIN;
    {
        SDL_Rect r;
        r.x = 0;
        r.y = y;
        r.h = 3 * R_MARGIN + font_size;
        r.w = game.size.ncol * (R_W + R_MARGIN) + R_MARGIN;
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &r);
    }


    if (p4_is_finished(game)) {
        if (p4_is_won(game)) {
            CASE_COLOR winner = p4_get_game_winner(game);
            if (winner == RED) {
                snprintf(text, font_size, "Red WON !");
            } else if (winner == YELLOW) {
                snprintf(text, font_size, "Yellow WON !");
            }
            render_text(ren, font, text, x, y, p4_repr(game.winner));
        } else {
            snprintf(text, font_size, "It's a tie");
            render_text(ren, font, text, x, y, p4_repr(NONE));
        }
    } else {
        CASE_COLOR active = p4_get_game_active(game);
        if (active == RED) {
            snprintf(text, font_size, "Red's turn");
        } else if (active == YELLOW) {
            snprintf(text, font_size, "Yellow's turn");
        } else {
            snprintf(text, font_size, "ERROR");
        }
        
        render_text(ren, font, text, x, y, p4_repr(game.active));
    }

}

static P4_Columns* p4_init_col(P4_Game game) {
    P4_Columns* columns = malloc(sizeof(P4_Columns));
    columns->c = malloc(game.size.ncol * sizeof(struct p4_column));
    columns->nb = game.size.ncol;
    
    SDL_Rect r;
    r.y = R_MARGIN;
    r.w = R_W;
    r.h = game.size.nrow * (R_W + R_MARGIN) - R_MARGIN;

    for (int i = 0; i < game.size.ncol; i++) {
        r.x = i * (R_W + R_MARGIN) + R_MARGIN;
        columns->c[i].c = r;
        columns->c[i].hover = false;
    }
    return columns;
}

static void p4_free_col(P4_Columns* columns) {
        free(columns->c);
        free(columns);
}

static void onHover(SDL_Event input, P4_Columns* columns) {
    SDL_Point p = {input.motion.x, input.motion.y};

    for (int i = 0; i < columns->nb; i++) {
        SDL_Rect r = columns->c[i].c;
        if (SDL_PointInRect(&p, &r)) {
            columns->c[i].hover = true;
        } else {
            columns->c[i].hover = false;
        }
    }
}

static void onClick(SDL_Event input, P4_Game* game, P4_Columns* columns) {
    SDL_Point p = {input.motion.x, input.motion.y};

    for (int i = 0; i < columns->nb; i++) {
        SDL_Rect r = columns->c[i].c;
        if (SDL_PointInRect(&p, &r)) {
            p4_game_step(game, i);
            break;
        }
    }
}

static void parse_arg(int argc, char* argv[], int* ncol, int* nrow, int* win_condition) {   
    if (argc > 1) {
        if (argc == 4) {          
            errno = 0;
            *ncol = strtol(argv[1], NULL, 10);
            if (errno != 0) {
                perror("strtol");
                exit(EXIT_FAILURE);
            }
            errno = 0;
            *nrow = strtol(argv[2], NULL, 10);
            if (errno != 0) {
                perror("strtol");
                exit(EXIT_FAILURE);
            }
            errno = 0;
            *win_condition = strtol(argv[3], NULL, 10);
            if (errno != 0) {
                perror("strtol");
                exit(EXIT_FAILURE);
            }
            if (*ncol < 1 || *nrow < 1 || *win_condition < 1) {
                fprintf(stderr, "ncol, nrow and win_condition must be supperior to 1\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "USAGE: %s ncol nrow win_condition\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        *ncol = 7;
        *nrow = 6;
        *win_condition = 4;
    }
}

int main(int argc, char* argv[]) {
    int ncol;
    int nrow;
    int win_condition;

    parse_arg(argc, argv, &ncol, &nrow, &win_condition);

    const int width = ncol * (R_W + R_MARGIN) + R_MARGIN;
    const int height = nrow * (R_W + R_MARGIN) + R_MARGIN + 25 + 2 * R_MARGIN;
    const float aratio = width / (height * 1.0);
    int window_width = 500 * aratio;
    int window_height = 500;

    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;
    SDL_Event input;
    bool quit = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"SDL Initialisation faillure\n");
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    int flags = SDL_WINDOW_RESIZABLE;
    SDL_CreateWindowAndRenderer(window_width, window_height, flags, &win, &ren);

    if (!win || !ren)
    {
        fprintf(stderr,"Window oppening faillure\n");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(win, "P4");

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("SourceSansPro-SemiBold.ttf", 25);
    if (font == NULL) {
        fprintf(stderr,"Error while loading font: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(ren, width, height);

    P4_Game* game = p4_init_game(ncol, nrow, win_condition);
    P4_Columns* columns = p4_init_col(*game);

    while (! quit) {        
        while (SDL_PollEvent(&input) > 0) {
            switch (input.type) {
            case SDL_QUIT: {
                quit = true;
            } break;
            case SDL_WINDOWEVENT: {
                if (input.window.event == SDL_WINDOWEVENT_RESIZED) {
                    window_width = input.window.data1;
                    window_height = input.window.data2;
                }
            } break;
            case SDL_MOUSEMOTION: {
                onHover(input, columns);
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                onClick(input, game, columns);
            } break;
            default: {
            }
            }
        }
        
        SDL_SetRenderDrawColor(ren, 53, 53, 53, 255);
        SDL_RenderClear(ren);

        p4_display_board(ren, *game);
        p4_display_columns(ren, *columns);
        p4_display_game_info(ren, font, *game);
        
        SDL_RenderPresent(ren);
        SDL_Delay(50);
    }
    
    p4_free_col(columns);
    p4_free_game(game);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifdef _MSC_VER
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>	
#endif

#include "p4.h"

//TODO: Use Surface for rendering the board

const int WIDTH  = 640;
const int HEIGHT = 480;

#define R_W 50
#define R_MARGIN 10

struct p4_column {
    bool hover;
    SDL_Rect c;
};

typedef struct p4_columns {
    struct p4_column c[BOARD_NC];
} P4_Columns;

SDL_Color p4_repr(CASE_COLOR color) {
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
// 0279c2

void p4_render_background(SDL_Renderer* ren) {
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.h = BOARD_NR * (R_W + R_MARGIN) + R_MARGIN;
    r.w = BOARD_NC * (R_W + R_MARGIN) + R_MARGIN;
    SDL_SetRenderDrawColor(ren, 2, 121, 194, 255);
    SDL_RenderFillRect(ren, &r);
}

void p4_display_board(SDL_Renderer* ren, P4_Game game) {
    SDL_Rect r;
    r.w = R_W;
    r.h = R_W;
    
    int pos_w = R_MARGIN;
    int pos_h = R_MARGIN;
    
    p4_render_background(ren);

    for (int j = BOARD_NR-1; j >= 0; j--) {
        for (int i = 0; i < BOARD_NC; i++) {
            r.x = pos_w;
            r.y = pos_h;
            
            SDL_Color color = p4_repr(game.board[i][j].p);

            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(ren, &r);
            
            if (is_won(game) && game.board[i][j].winning_move) {
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

void p4_display_columns(SDL_Renderer* ren, P4_Columns columns) {
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 50);
    for (int i = 0; i < BOARD_NC; i++) {
        if (columns.c[i].hover) {
            SDL_RenderFillRect(ren, &(columns.c[i].c));            
        }
    }
}

//TODO: prebake the textures
void render_text(SDL_Renderer* ren, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_Rect r = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ren, texture, NULL, &r);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void p4_display_game_info(SDL_Renderer* ren, TTF_Font* font, P4_Game game) {
    char text[20];
    
    int x = R_MARGIN;
    int y = BOARD_NR * (R_W + R_MARGIN) + R_MARGIN;


    if (is_won(game)) {
        CASE_COLOR winner = get_game_winner(game);
        if (winner == RED) {
            snprintf(text, 19, "Red WINS !");
        } else if (winner == YELLOW) {
            snprintf(text, 19, "Yellow WINS !");
        }
        render_text(ren, font, text, x, y, p4_repr(game.winner));
    } else {
        CASE_COLOR active = get_game_active(game);
        if (active == RED) {
            snprintf(text, 19, "Red turn");
        } else if (active == YELLOW) {
            snprintf(text, 19, "Yellow turn");
        } else {
            snprintf(text, 19, "ERROR");
        }
        
        render_text(ren, font, text, x, y, p4_repr(game.active));
    }

}

void p4_init_col_rect(P4_Columns* columns) {
    SDL_Rect r;
    r.y = R_MARGIN;
    r.w = R_W;
    r.h = BOARD_NR * (R_W + R_MARGIN) - R_MARGIN;

    for (int i = 0; i < BOARD_NC; i++) {
        r.x = i * (R_W + R_MARGIN) + R_MARGIN;
        columns->c[i].c = r;
        columns->c[i].hover = false;
    }
}

void onHover(SDL_Event input, P4_Columns* columns) {
    SDL_Point p = {input.motion.x, input.motion.y};
    //printf("point %d %d\n", p.x, p.y);

    for (int i = 0; i < BOARD_NC; i++) {
        SDL_Rect r = columns->c[i].c;
        if (SDL_PointInRect(&p, &r)) {
            columns->c[i].hover = true;
        } else {
            columns->c[i].hover = false;
        }
    }
}

void onClick(SDL_Event input, P4_Game* game, P4_Columns* columns) {
    SDL_Point p = {input.motion.x, input.motion.y};
    for (int i = 0; i < BOARD_NC; i++) {
        if (! is_won(*game)) {
            SDL_Rect r = columns->c[i].c;
            if (SDL_PointInRect(&p, &r)) {
                if (insert_in_col(game, i)) {
                    game_switch(game);
                }
                break;
            }
        } else {
            reset_game(game);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;
    SDL_Event input;
    
    P4_Game game;
    P4_Columns columns;
    
    bool quit = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"SDL Initialisation faillure\n");
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    int flags = 0;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, flags, &win, &ren);

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

    p4_init_col_rect(&columns);
    init_game(&game);
    game.active = RED; // RED begins

    while (! quit) {        
        while (SDL_PollEvent(&input) > 0) {
            switch (input.type) {
            case SDL_QUIT: {
                quit = true;
            } break;
            case SDL_MOUSEMOTION: {
                onHover(input, &columns);
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                onClick(input, &game, &columns);
            } break;
            default: {
            }
            }
        }
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        p4_display_board(ren, game);
        p4_display_columns(ren, columns);
        p4_display_game_info(ren, font, game);
        

        
        SDL_RenderPresent(ren);
        SDL_Delay(50);
    }
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    return EXIT_SUCCESS;
}

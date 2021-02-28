#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "p4.h"

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

typedef struct p4_game {
    CASE_COLOR active;
} P4_Game;

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

void p4_display_board(SDL_Renderer* ren, P4_Board board) {
    SDL_Rect r;
    r.w = R_W;
    r.h = R_W;
    
    int pos_w = R_MARGIN;
    int pos_h = R_MARGIN;
    
    for (int j = BOARD_NR-1; j >= 0; j--) {
        for (int i = 0; i < BOARD_NC; i++) {
            r.x = pos_w;
            r.y = pos_h;
            
            SDL_Color color = p4_repr(board.b[i][j].p);

            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(ren, &r);

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

void game_switch(P4_Game* game) {
    if (game->active == RED) {
        game->active = YELLOW;
    } else if (game->active == YELLOW) {
        game->active = RED;
    } else {
        assert(false);
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

void onClick(SDL_Event input, P4_Game* game, P4_Board* board, P4_Columns* columns) {
    SDL_Point p = {input.motion.x, input.motion.y};
    for (int i = 0; i < BOARD_NC; i++) {
        SDL_Rect r = columns->c[i].c;
        if (SDL_PointInRect(&p, &r)) {
            if (insert_in_col(board, i, game->active)) {
                game_switch(game);
            }
            break;
        }
    }
}

int main() {
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;
    SDL_Event input;
    
    P4_Game game;
    P4_Board board;
    P4_Columns columns;
    
    bool quit = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"Erreur à l'initialisation de SDL\n");
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    int flags = 0;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, flags, &win, &ren);

    if (!win || !ren)
    {
        fprintf(stderr,"Erreur à la création de la fenêtre\n");
        return EXIT_FAILURE;
    }


    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    p4_init_col_rect(&columns);
    init_board(&board);
    game.active = RED; // RED begins
    debug_print_board(board);
    insert_in_col(&board, 0, RED);
    debug_print_board(board);

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
                onClick(input, &game, &board, &columns);
            } break;
            default: {
            }
            }
        }
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        p4_display_board(ren, board);
        p4_display_columns(ren, columns);
        

        
        SDL_RenderPresent(ren);
        SDL_Delay(50);
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    return EXIT_SUCCESS;
}

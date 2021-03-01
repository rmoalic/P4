#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "p4.h"

bool check_win_on_insert(P4_Game* game, int col, int row);

void init_board(P4_Game* game) {
    for (int i = 0; i < BOARD_NC; i++) {
        for (int j = 0; j < BOARD_NR; j++) {
            game->board[i][j].p = NONE;
        }
    }
    game->active = RED;
    game->winner = NONE;
}

bool change_case_color(P4_Game* game, int col, int row, CASE_COLOR color) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    assert(row >= 0 && row < BOARD_NR);
    
    P4_Case* bcase = &(game->board[col][row]);
    bool changed = false;
    
    if (bcase->p == NONE) {
        bcase->p = color;
        changed = true;
    } else {
        changed = false;
    }
    return changed;
}

int find_first_available_position_in_col(P4_Game* game, int col) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    
    int pos = -1;
    P4_Case* r_case = game->board[col];
    
    for (int i = 0; i < BOARD_NR; i++) {
        if (r_case[i].p == NONE) {
            pos = i;
            break;
        }
    }
    return pos;
}

bool insert_in_col_impl(P4_Game* game, int col, CASE_COLOR color) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    
    bool inserted = false;
    int row = find_first_available_position_in_col(game, col);
    if (row < 0) {
        printf("Column %d is full\n", col);
        inserted = false;
    } else {
        change_case_color(game, col, row, color);
        bool win = check_win_on_insert(game, col, row);
        if (win) {
            game->winner = color;
        }
        printf("win: %d\n", win);
        inserted = true;
    }
    
    return inserted;
}

bool insert_in_col(P4_Game* game, int col) {
    return insert_in_col_impl(game, col, game->active);
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

bool check_chain(P4_Game* game, int col, int row, int vcol, int vrow) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    assert(row >= 0 && row < BOARD_NR);
    assert(vcol >= -1 && vcol <= 1);
    assert(vrow >= -1 && vrow <= 1);
    assert(! (vrow == 0 && !(vcol != 0)));
    assert(! (vcol == 0 && !(vrow != 0)));
    
    printf("CHECK %d %d\n", vcol, vrow);
        
    bool ret = false;
    bool hope = true;
    bool inv = false;
    int acc = 1;
    int ncol = col;
    int nrow = row;

    
    CASE_COLOR color = game->board[col][row].p;
    CASE_COLOR ncolor;
    printf("\tstart %d %d\n", col, row);
    while (hope && acc < 4) {
        ncol = ncol + (vcol * (inv ? -1 : 1));
        nrow = nrow + (vrow * (inv ? -1 : 1));                    

        printf("\tcheck %d %d \n", ncol, nrow);

        // Bound Check for row and col, if out of bound, reverse vector 
        // direction and set current position to the start of the chain
        // in order to recalculate new positions.
        if ((nrow < 0 || nrow >= BOARD_NR) || (ncol < 0 || ncol >= BOARD_NC)) {
            if (! inv) {
                inv = true;
                nrow = row;
                ncol = col;
            } else {
                hope = false;
            }
            printf("\tBound check rev\n");
            continue;
        }

        //assert(col != ncol && row != nrow);
        
        ncolor = game->board[ncol][nrow].p;
        
        // Color check, if the color matches, continue the chain.
        // Otherwise if we did not already reverse the vector, we
        // reverse it, if no more piece of the right color can be
        // found, it is not a win.
        if (ncolor == color) {
            acc = acc + 1;
            printf("\tvalidate %d, %d \n", ncol, nrow);
        } else {
              if (! inv) {
                  printf("\t inv col<>\n");
                  inv = true;
                  ncol = col;
                  nrow = row;
              } else {
                  hope = false;
                  ret = false;
              }
        }
    }
    printf("acc: %d\n\n", acc);
    if (acc == 4) {
        ret = true;
    }
    
    return ret;
}

bool check_win_on_insert(P4_Game* game, int col, int row) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    assert(row >= 0 && row < BOARD_NR);
    
    return check_chain(game, col, row,  0, -1) ||
           check_chain(game, col, row,  1,  0) ||
           check_chain(game, col, row, -1, -1) ||
           check_chain(game, col, row,  1, -1);
}

char repr_color(CASE_COLOR color) {
    char c;
    switch (color){
        case NONE: {
            c = '.';
        }
        break;
        case RED: {
            c = 'R';
        }
        break;
        case YELLOW: {
            c = 'Y';
        }
        break;
        default: {
            c = '@';
        }
    }
    return c;
}

void debug_print_board(P4_Game game) {
    printf("board--------------------------\n");
    for (int j = BOARD_NR-1; j >= 0; j--) {
        for (int i = 0; i < BOARD_NC; i++) {
            printf("%c", repr_color(game.board[i][j].p));
        }
        printf("\n");
    }
    printf("end board----------------------\n");
}

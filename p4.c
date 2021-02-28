#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "p4.h"

void init_board(P4_Game* game) {
    for (int i = 0; i < BOARD_NC; i++) {
        for (int j = 0; j < BOARD_NR; j++) {
            game->board[i][j].p = NONE;
        }
    }
    game->active = RED;
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

bool check_win_on_insert(P4_Game* game, int col, int row) {
    assert(game != NULL);
    assert(col >= 0 && col < BOARD_NC);
    assert(row >= 0 && row < BOARD_NR);
    
    //CASE_COLOR color = board->b[col][row].p;
    
    if (col > 0 && col < BOARD_NC-1) {
        // check left and right
        
    } else if (col > 0) {
        // check left
    
    } else if (col < BOARD_NC-1) {
        //check right
        
    } else {
        assert(false);
    }
    
    return false;
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

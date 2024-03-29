#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "p4.h"

static bool p4_check_win_on_insert(P4_Game* game, int col, int row);

P4_Game* p4_init_game(int ncol, int nrow, int win_condition) {
    assert(ncol > 0);
    assert(nrow > 0);
    assert(win_condition > 0);

    P4_Game tmp = {.size = {ncol, nrow}, 
                   .win_condition = win_condition};
    P4_Game* game = malloc(sizeof(P4_Game));
    memcpy(game, &tmp, sizeof(P4_Game));

    game->board = malloc(ncol * sizeof(P4_Case*));
    for (int i = 0; i < ncol; i++) {
        game->board[i] = malloc(nrow * sizeof(P4_Case));
    }

    p4_reset_game(game);

    return game;
}

void p4_free_game(P4_Game* game) {
    assert(game != NULL);

    for (int i = 0; i < game->size.ncol; i++) {
        free(game->board[i]);
    }
    free(game->board);
    free(game);
}

void p4_reset_game(P4_Game* game) {
    assert(game != NULL);

    for (int i = 0; i < game->size.ncol; i++) {
        for (int j = 0; j < game->size.nrow; j++) {
            game->board[i][j].p = NONE;
            game->board[i][j].winning_move = false;
        }
    }
    game->active = RED;
    game->winner = NONE;
    game->remaining_pieces = game->size.ncol * game->size.nrow;
}

bool p4_is_won(P4_Game game) {
    return game.winner != NONE;
}

bool p4_is_finished(P4_Game game) {
    return p4_is_won(game) || game.remaining_pieces <= 0;
}

CASE_COLOR p4_get_game_winner(P4_Game game) {
    assert(p4_is_won(game));

    return game.winner;
}

CASE_COLOR p4_get_game_active(P4_Game game) {
    assert(! p4_is_won(game));

    return game.active;
}

P4_Board_Size p4_get_game_size(P4_Game game) {
    return game.size;
}

static bool p4_change_case_color(P4_Game* game, int col, int row, CASE_COLOR color) {
    assert(game != NULL);
    assert(col >= 0 && col < game->size.ncol);
    assert(row >= 0 && row < game->size.nrow);

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

static int p4_find_first_available_position_in_col(P4_Game* game, int col) {
    assert(game != NULL);
    assert(col >= 0 && col < game->size.ncol);

    int pos = -1;
    P4_Case* r_case = game->board[col];
    
    for (int i = 0; i < game->size.nrow; i++) {
        if (r_case[i].p == NONE) {
            pos = i;
            break;
        }
    }
    return pos;
}

static bool p4_insert_in_col_impl(P4_Game* game, int col, CASE_COLOR color) {
    assert(game != NULL);
    assert(game->remaining_pieces > 0);
    assert(col >= 0 && col < game->size.ncol);

    bool inserted = false;
    int row = p4_find_first_available_position_in_col(game, col);
    if (row < 0) {
        printf("Column %d is full\n", col);
        inserted = false;
    } else {
        p4_change_case_color(game, col, row, color);
        bool win = p4_check_win_on_insert(game, col, row);
        if (win) {
            game->winner = color;
        }
        printf("win: %d\n", win);
        game->remaining_pieces = game->remaining_pieces - 1;
        inserted = true;
    }

    return inserted;
}

static bool p4_insert_in_col(P4_Game* game, int col) {
    return p4_insert_in_col_impl(game, col, game->active);
}

static void p4_game_switch(P4_Game* game) {
    assert(game != NULL);

    if (game->active == RED) {
        game->active = YELLOW;
    } else if (game->active == YELLOW) {
        game->active = RED;
    } else {
        assert(false);
    }
}

void p4_game_step(P4_Game* game, int col) {
    if (! p4_is_finished(*game)) {
        if (p4_insert_in_col(game, col)) {
            p4_game_switch(game);
        }
    } else {
        p4_reset_game(game);
    }
}

static void p4_set_winning_point(P4_Game* game, int col, int row) {
    assert(game != NULL);
    assert(col >= 0 && col < game->size.ncol);
    assert(row >= 0 && row < game->size.nrow);

    game->board[col][row].winning_move = true;
}

static void p4_set_winning_chain(P4_Game* game, struct Point* chain, int size) {
    assert(game != NULL);
    assert(chain != NULL);
    assert(size >= 0);

    for (int i = 0; i < size; i++) {
        p4_set_winning_point(game, chain[i].x, chain[i].y);
    }
}

static struct Point* p4_find_chain(P4_Game* game, int col, int row, int vcol, int vrow, int size) {
    assert(game != NULL);
    assert(col >= 0 && col < game->size.ncol);
    assert(row >= 0 && row < game->size.nrow);
    assert(vcol >= -1 && vcol <= 1);
    assert(vrow >= -1 && vrow <= 1);
    assert(! (vrow == 0 && !(vcol != 0)) && "si vrow == 0 alors vcol != 0");
    assert(! (vcol == 0 && !(vrow != 0)) && "si vcol == 0 alors vrow != 0");
    assert(size >= 0);

    printf("CHECK %d %d\n", vcol, vrow);

    bool hope = true;
    bool inv = false;
    int acc = 1;
    int ncol = col;
    int nrow = row;

    struct Point* winning_points = malloc(size * sizeof(struct Point));

    CASE_COLOR color = game->board[col][row].p;
    CASE_COLOR ncolor;
    printf("\tstart %d %d\n", col, row);
    winning_points[acc - 1].x = col;
    winning_points[acc - 1].y = row;
    while (hope && acc < size) {
        ncol = ncol + (vcol * (inv ? -1 : 1));
        nrow = nrow + (vrow * (inv ? -1 : 1));                    

        printf("\tcheck %d %d \n", ncol, nrow);

        // Bound Check for row and col, if out of bound, reverse vector 
        // direction and set current position to the start of the chain
        // in order to recalculate new positions.
        if ((nrow < 0 || nrow >= game->size.nrow) || (ncol < 0 || ncol >= game->size.ncol)) {
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

        assert(! (col == ncol && row == nrow) && "Don't count the starting point twice");

        ncolor = game->board[ncol][nrow].p;

        // Color check, if the color matches, continue the chain.
        // Otherwise if we did not already reverse the vector, we
        // reverse it, if no more piece of the right color can be
        // found, it is not a win.
        if (ncolor == color) {
            acc = acc + 1;
            printf("\tvalidate %d, %d \n", ncol, nrow);
            winning_points[acc - 1].x = ncol;
            winning_points[acc - 1].y = nrow;
        } else {
              if (! inv) {
                  printf("\t inv col<>\n");
                  inv = true;
                  ncol = col;
                  nrow = row;
              } else {
                  hope = false;
              }
        }
    }
    printf("acc: %d\n\n", acc);

    if (acc != size) {
        free(winning_points);
        winning_points = NULL;
    }

    return winning_points;
}

static bool p4_find_and_tag_chain(P4_Game* game, int col, int row, int vcol, int vrow, int size) {
    struct Point* chain = NULL;
    bool found = false;

    chain = p4_find_chain(game, col, row,  vcol, vrow, size);
    if (chain != NULL) {
        found = true;
        p4_set_winning_chain(game, chain, size);
        free(chain);
    }
    return found;
}

static bool p4_check_win_on_insert(P4_Game* game, int col, int row) {
    assert(game != NULL);
    assert(col >= 0 && col < game->size.ncol);
    assert(row >= 0 && row < game->size.nrow);

    int size = game->win_condition;
    bool found = false;

    found |= p4_find_and_tag_chain(game, col, row,  0, -1, size);
    found |= p4_find_and_tag_chain(game, col, row,  1,  0, size);
    found |= p4_find_and_tag_chain(game, col, row, -1, -1, size);
    found |= p4_find_and_tag_chain(game, col, row,  1, -1, size);

    return found;
}

char p4_repr_color(CASE_COLOR color) {
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

void p4_debug_print_board(P4_Game game) {
    printf("board--------------------------\n");
    for (int j = game.size.nrow-1; j >= 0; j--) {
        for (int i = 0; i < game.size.ncol; i++) {
            printf("%c", p4_repr_color(game.board[i][j].p));
        }
        printf("\n");
    }
    printf("end board----------------------\n");
}

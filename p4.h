
#ifndef include_p4
#define include_p4 1

typedef enum CaseColor {
    NONE = 0,
    RED,
    YELLOW
} CASE_COLOR;

typedef struct p4_case {
    CASE_COLOR p;
    bool winning_move;
} P4_Case;

struct Point {
    int x;
    int y;
};

typedef struct p4_board_size {
    const int ncol;
    const int nrow;
} P4_Board_Size;

typedef struct p4_game {
    P4_Case** board;
    const struct p4_board_size size;
    const int win_condition;
    CASE_COLOR active;
    CASE_COLOR winner;
    int remaining_pieces;
} P4_Game;

P4_Game* p4_init_game(int ncol, int nrow, int win_condition);

void p4_reset_game(P4_Game* game);

void p4_free_game(P4_Game* game);

bool p4_is_finished(P4_Game game);

bool p4_is_won(P4_Game game);

CASE_COLOR p4_get_game_winner(P4_Game game);

CASE_COLOR p4_get_game_active(P4_Game game);

P4_Board_Size p4_get_game_size(P4_Game game);

void p4_game_step(P4_Game* game, int col);

char p4_repr_color(CASE_COLOR color);

void p4_debug_print_board(P4_Game game);

#endif

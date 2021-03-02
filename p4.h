
#ifndef include_p4
#define include_p4 1

#define WIN_CONDITION 4

typedef enum CaseColor {
    NONE,
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
    const struct p4_board_size size;
    P4_Case** board;
    CASE_COLOR active;
    CASE_COLOR winner;
    int remaining_pieces;
} P4_Game;

P4_Game* init_game(int ncol, int nrow);

void reset_game(P4_Game* game);

void free_game(P4_Game* game);

bool is_finished(P4_Game game);

bool is_won(P4_Game game);

CASE_COLOR get_game_winner(P4_Game game);

CASE_COLOR get_game_active(P4_Game game);

P4_Board_Size get_game_size(P4_Game game);

bool insert_in_col(P4_Game* game, int col);

void game_switch(P4_Game* game);

void debug_print_board(P4_Game game);

#endif

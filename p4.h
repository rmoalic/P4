
#ifndef include_p4
#define include_p4 1

#define BOARD_NR 6
#define BOARD_NC 7
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

typedef struct p4_game {
    P4_Case board[BOARD_NC][BOARD_NR];
    CASE_COLOR active;
    CASE_COLOR winner;
} P4_Game;

void init_game(P4_Game* game);

void reset_game(P4_Game* game);

bool is_won(P4_Game game);

CASE_COLOR get_game_winner(P4_Game game);

CASE_COLOR get_game_active(P4_Game game);

bool insert_in_col(P4_Game* game, int col);

void game_switch(P4_Game* game);

void debug_print_board(P4_Game game);

#endif

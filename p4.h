
#ifndef include_p4
#define include_p4 1

#define BOARD_NR 6
#define BOARD_NC 7

typedef enum CaseColor {
    NONE,
    RED,
    YELLOW
} CASE_COLOR;

typedef struct p4_case {
    CASE_COLOR p;
} P4_Case;

typedef struct p4_board {
    P4_Case b[BOARD_NC][BOARD_NR];
} P4_Board;

void init_board(P4_Board* board);

bool insert_in_col(P4_Board* board, int col, CASE_COLOR color);

void debug_print_board(P4_Board board);

#endif

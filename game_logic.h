#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>

#define BOARD_SIZE 15


typedef enum {
    EMPTY,
    BLACK,
    WHITE
} ChessType;


typedef enum {
    EASY,
    MEDIUM,
    HARD,
    EXPERT
} AIDifficulty;


extern ChessType board[BOARD_SIZE][BOARD_SIZE];


void init_board();
bool check_win(int x, int y, ChessType type, ChessType board[BOARD_SIZE][BOARD_SIZE]);

#endif

#include <game_logic.h>
#include <cstring>

ChessType board[BOARD_SIZE][BOARD_SIZE];


void init_board() {
    memset(board, EMPTY, sizeof(board));
}


bool check_win(int x, int y, ChessType type, ChessType board[BOARD_SIZE][BOARD_SIZE])
{
    int count = 0;


    count = 1;

    for (int i = x-1; i >= 0 && board[i][y] == type; i--) count++;

    for (int i = x+1; i < BOARD_SIZE && board[i][y] == type; i++) count++;
    if (count >= 6) return true;


    count = 1;
    for (int i = y-1; i >= 0 && board[x][i] == type; i--) count++;
    for (int i = y+1; i < BOARD_SIZE && board[x][i] == type; i++) count++;
    if (count >= 6) return true;


    count = 1;
    for (int i = x-1, j = y-1; i >=0 && j >=0 && board[i][j] == type; i--,j--) count++;
    for (int i = x+1, j = y+1; i < BOARD_SIZE && j < BOARD_SIZE && board[i][j] == type; i++,j++) count++;
    if (count >= 6) return true;


    count = 1;
    for (int i = x+1, j = y-1; i < BOARD_SIZE && j >=0 && board[i][j] == type; i++,j--) count++;
    for (int i = x-1, j = y+1; i >=0 && j < BOARD_SIZE && board[i][j] == type; i--,j++) count++;
    if (count >= 6) return true;

    return false;
}

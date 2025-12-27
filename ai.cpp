#include "ai.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern ChessType board[BOARD_SIZE][BOARD_SIZE];


static void init_rand() {
    static bool inited = false;
    if (!inited) {
        srand((unsigned int)time(NULL));
        inited = true;
    }
}


int count_continuous(int x, int y, ChessType player, int dx, int dy) {
    int count = 0;
    int cx = x + dx;
    int cy = y + dy;
    while (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE) {
        if (board[cx][cy] == player) {
            count++;
            cx += dx;
            cy += dy;
        } else {
            break;
        }
    }
    return count;
}


int score_position(int x, int y, ChessType player) {
    if (board[x][y] != EMPTY) return -1;

    int score = 0;
    int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    ChessType enemy = (player == BLACK) ? WHITE : BLACK;


    for (int d = 0; d < 4; d++) {
        int cnt = count_continuous(x, y, player, dirs[d][0], dirs[d][1]);

        int cnt_rev = count_continuous(x, y, player, -dirs[d][0], -dirs[d][1]);
        int total = cnt + cnt_rev + 1;

        if (total >= 6) score += 10000;
        else if (total == 5) score += 1000;
        else if (total == 4) score += 100;
        else if (total == 3) score += 10;
        else if (total == 2) score += 2;
    }


    for (int d = 0; d < 4; d++) {
        int cnt = count_continuous(x, y, enemy, dirs[d][0], dirs[d][1]);
        int cnt_rev = count_continuous(x, y, enemy, -dirs[d][0], -dirs[d][1]);
        int total = cnt + cnt_rev + 1;

        if (total >= 6) score += 9000;
        else if (total == 5) score += 900;
        else if (total == 4) score += 90;
        else if (total == 3) score += 9;
    }


    int centerX = BOARD_SIZE / 2;
    int centerY = BOARD_SIZE / 2;
    int dist = abs(x - centerX) + abs(y - centerY);
    score += (BOARD_SIZE - dist) * 3;


    if (x == 0 || x == BOARD_SIZE-1 || y == 0 || y == BOARD_SIZE-1) {
        score -= 5;
    }

    return score;
}


void ai_easy(int *x, int *y) {
    init_rand();
    do {
        *x = rand() % BOARD_SIZE;
        *y = rand() % BOARD_SIZE;
    } while (board[*x][*y] != EMPTY);
}


void ai_medium(int *x, int *y) {

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int score = score_position(i, j, WHITE);
                if (score >= 9000) {
                    *x = i; *y = j;
                    return;
                }
            }
        }
    }
    ai_easy(x, y);
}


void ai_hard(int *x, int *y) {

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int score = score_position(i, j, BLACK);
                if (score >= 10000) {
                    *x = i; *y = j;
                    return;
                }
            }
        }
    }

    ai_medium(x, y);
}


void ai_expert(int *x, int *y) {
    int bestScore = -1;
    *x = -1; *y = -1;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int currentScore = score_position(i, j, BLACK);
                if (currentScore > bestScore) {
                    bestScore = currentScore;
                    *x = i; *y = j;
                }
            }
        }
    }


    if (*x == -1 || *y == -1) {
        ai_easy(x, y);
    }
}


void ai_play(AIDifficulty diff) {
    int x, y;
    switch (diff) {
    case EASY: ai_easy(&x, &y); break;
    case MEDIUM: ai_medium(&x, &y); break;
    case HARD: ai_hard(&x, &y); break;
    case EXPERT: ai_expert(&x, &y); break;
    default: ai_easy(&x, &y); break;
    }
    board[x][y] = BLACK;
}

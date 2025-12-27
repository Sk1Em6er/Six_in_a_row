#ifndef AI_H
#define AI_H

#include "game_logic.h"



void ai_expert(int *x, int *y);

int score_position(int x, int y, ChessType player);


void ai_play(AIDifficulty diff);
void ai_easy(int *x, int *y);
void ai_medium(int *x, int *y);
void ai_hard(int *x, int *y);
int count_continuous(int x, int y, ChessType player, int dx, int dy);

#endif // AI_H

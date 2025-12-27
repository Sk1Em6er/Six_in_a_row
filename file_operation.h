#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include <time.h>
#include <stdbool.h>
#include "game_logic.h"


typedef struct {
    char gameId[32];
    time_t createTime;
    ChessType winner;
    int stepCount;
} GameRecord;

// 保存/加载游戏记录
bool save_game_record(const GameRecord *record);
void load_game_record(const char *gameId, GameRecord *record);
bool delete_game_record(const char *gameId);
#endif // FILE_OPERATION_H

#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include "file_operation.h"
#include <stdbool.h>


bool json_write_game_record(const char *filePath, const GameRecord *record);
bool json_read_game_record(const char *filePath, GameRecord *record);

bool delete_game_record(const char *gameId);

#endif // DATA_PERSISTENCE_H

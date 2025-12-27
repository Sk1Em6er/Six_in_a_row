#include "data_persistence.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>


bool json_write_game_record(const char *filePath, const GameRecord *record) {
    if (!filePath || !record) return false;
    
    FILE *fp = fopen(filePath, "w");
    if (!fp) return false;
    
    fprintf(fp, "{\n");
    fprintf(fp, "  \"gameId\": \"%s\",\n", record->gameId);
    fprintf(fp, "  \"createTime\": %lld,\n", (long long)record->createTime);
    fprintf(fp, "  \"winner\": %d,\n", record->winner);
    fprintf(fp, "  \"stepCount\": %d\n", record->winner);
    fprintf(fp, "}\n");
    
    fclose(fp);
    return true;
}


bool json_read_game_record(const char *filePath, GameRecord *record) {
    if (!filePath || !record) return false;
    
    FILE *fp = fopen(filePath, "r");
    if (!fp) return false;
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "gameId")) {
            sscanf(line, "  \"gameId\": \"%[^\"]\",", record->gameId);
        } else if (strstr(line, "createTime")) {
            sscanf(line, "  \"createTime\": %lld,", (long long*)&record->createTime);
        } else if (strstr(line, "winner")) {
            sscanf(line, "  \"winner\": %d,", (int*)&record->winner);
        } else if (strstr(line, "stepCount")) {
            sscanf(line, "  \"stepCount\": %d", &record->stepCount);
        }
    }
    
    fclose(fp);
    return true;
}


#include "file_operation.h"
#include <string.h>
#include <windows.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fstream>
#include <string>

static void create_record_dir() {
#ifdef _WIN32
    mkdir("game_records");  // Windows
#else
    mkdir("game_records", 0755);  // Linux/Mac
#endif
}

bool delete_game_record(const char *gameId) {
    create_record_dir();
    if (gameId == NULL || strlen(gameId) == 0) {
        return false;
    }


    char filePath[256];
    snprintf(filePath, sizeof(filePath), "game_records/%s.txt", gameId);


#ifdef _WIN32
    return (remove(filePath) == 0);  // Windows
#else
    return (unlink(filePath) == 0);  // Linux/Mac
#endif
}


bool save_game_record(const GameRecord *record) {
    create_record_dir();
    if (!record) return false;
    

    CreateDirectoryA("game_records", NULL);
    

    char filePath[256];
    snprintf(filePath, sizeof(filePath), "game_records/%s.txt", record->gameId);
    
    FILE *fp = fopen(filePath, "w");
    if (!fp) return false;
    

    fprintf(fp, "GameID: %s\n", record->gameId);
    fprintf(fp, "CreateTime: %lld\n", (long long)record->createTime);
    fprintf(fp, "Winner: %d\n", record->winner);
    fprintf(fp, "StepCount: %d\n", record->stepCount);
    
    fclose(fp);
    return true;
}


void load_game_record(const char *gameId, GameRecord *record) {
    if (!gameId || !record) return;
    create_record_dir();

    std::string filePath = "game_records/";
    filePath += gameId;
    filePath += ".txt";

    std::ifstream fs(filePath);
    if (!fs.is_open()) return;

    memset(record, 0, sizeof(GameRecord));
    std::string key;

    fs >> key >> record->gameId;
    if (!fs) { fs.close(); return; }

    fs >> key >> record->createTime;
    if (!fs) { fs.close(); return; }

    fs >> key >> (int&)record->winner;
    if (!fs) { fs.close(); return; }

    fs >> key >> record->stepCount;
    if (!fs) { fs.close(); return; }

    fs.close();
}


#include "level.h"
#include "state.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
using namespace std;

static bool isEntitySymbol(char c)
{
    return c == '<' || c == ')' || c == '\\' ||
           c == '4' || c == '*' || c == '~' ||
           c == ';' || c == '(' || c == '!' ||
           c == '%' || c == '$' || c == '2' ||
           c == '7' || c == '^' || c == '@' ||
           c == '&';
}

bool loadBuildingMap(
    const char* filename,
    char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
    char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);
    if (!file)
        return false;
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            entityCounts[f][r] = 0;
            for (int e = 0; e < MAX_ENTITIES_PER_ROOM; e++)
                roomEntities[f][r][e] = '\0';
            if (r < ROOMS_PER_FLOOR - 1)
                horizontalConn[f][r] = ' ';
        }
    }
    bool floorsLoaded[NUM_FLOORS] = {false};
    char line[MAX_LINE_LEN];
    char entityLine[MAX_LINE_LEN];
    while (file.getline(line, MAX_LINE_LEN))
    {
        bool floorRow = true;
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            int pos = 10 + (r * 12);
            if (line[pos] != 'F' ||
                line[pos + 2] != '-' ||
                line[pos + 3] != 'R' ||
                line[pos + 4] != ('1' + r))
            {
                floorRow = false;
                break;
            }
        }
        if (!floorRow)
            continue;
        int floorNumber = line[11] - '0';
        if (floorNumber < 1 || floorNumber > NUM_FLOORS)
            return false;
        int floor = floorNumber - 1;
        if (floorsLoaded[floor])
            return false;
        if (!file.getline(entityLine, MAX_LINE_LEN))
            return false;
        if (strlen(entityLine) <= 56)
            return false;
        for (int r = 0; r < ROOMS_PER_FLOOR - 1; r++)
            horizontalConn[floor][r] = line[20 + (r * 12)];
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            int start = 9 + (r * 12);
            for (int i = 0; i < 12; i++)
            {
                char symbol = entityLine[start + i];
                if (isEntitySymbol(symbol))
                {
                    if (entityCounts[floor][r] >= MAX_ENTITIES_PER_ROOM)
                        return false;
                    roomEntities[floor][r][entityCounts[floor][r]] = symbol;
                    entityCounts[floor][r]++;
                }
            }
        }
        floorsLoaded[floor] = true;
    }
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        if (!floorsLoaded[f])
            return false;
    }
    return true;
}

bool loadInitialState(
    const char* filename,
    int& playerFloor, int& playerRoom,
    int& energy, int& time, int& score,
    char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
    bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft)
{
    ifstream file(filename);
    if (!file)
        return false;
    playerFloor = 1;
    playerRoom = 1;
    energy = INITIAL_ENERGY;
    time = INITIAL_TIME;
    score = INITIAL_SCORE;
    inventoryCount = 0;
    emergencyTripsLeft = 0;
    for (int i = 0; i < MAX_INVENTORY_CAPACITY; i++)
        inventory[i] = '\0';
    for (int i = 0; i < NUM_OBJECTIVES; i++)
        objectives[i] = false;
    char line[MAX_LINE_LEN];
    while (file.getline(line, MAX_LINE_LEN))
    {
        char* separator = strchr(line, ':');
        if (separator == NULL)
            continue;
        *separator = '\0';
        char* key = line;
        char* value = separator + 1;
        while (*value == ' ')
            value++;
        int length = strlen(value);
        while (length > 0 &&
              (value[length - 1] == '\r' || value[length - 1] == ' '))
        {
            value[length - 1] = '\0';
            length--;
        }
        if (strcmp(key, "INITIAL_ENERGY") == 0)
            energy = atoi(value);
        else if (strcmp(key, "INITIAL_TIME") == 0)
            time = atoi(value);
        else if (strcmp(key, "INITIAL_SCORE") == 0)
            score = atoi(value);
        else if (strcmp(key, "START_ROOM") == 0)
        {
            if (strcmp(value, "PLAYER_SELECTS") == 0)
            {
                playerFloor = 1;
                playerRoom = 1;
            }
            else if (sscanf(value, "F%d-R%d",
                            &playerFloor, &playerRoom) != 2)
                return false;
        }
        else if (strcmp(key, "PASSENGER_RESCUED") == 0)
            objectives[OBJ_PASSENGER_RESCUED] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "CAT_RESCUED") == 0)
            objectives[OBJ_CAT_SECURED] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "CABINET_UNLOCKED") == 0)
            objectives[OBJ_CABINET_UNLOCKED] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "TRAIN_KEY_COLLECTED") == 0)
            objectives[OBJ_TRAIN_KEY] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "FUSE_INSTALLED") == 0)
        {
            objectives[OBJ_FUSE_INSTALLED] =
                (strcmp(value, "YES") == 0);
            objectives[OBJ_FUSE_COLLECTED] =
                objectives[OBJ_FUSE_INSTALLED];
        }
        else if (strcmp(key, "LIFT_PASS_COLLECTED") == 0)
            objectives[OBJ_LIFT_PASS] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "TRAIN_PREPARED") == 0)
            objectives[OBJ_TRAIN_PREPARED] =
                (strcmp(value, "YES") == 0);
        else if (strcmp(key, "CLUES_COLLECTED") == 0)
        {
            if (strchr(value, '4') != NULL)
                objectives[OBJ_CLUE_4] = true;
            if (strchr(value, '2') != NULL)
                objectives[OBJ_CLUE_2] = true;
            if (strchr(value, '7') != NULL)
                objectives[OBJ_CLUE_7] = true;
        }
    }
    if (objectives[OBJ_FUSE_INSTALLED] &&
        objectives[OBJ_LIFT_PASS])
    {
        objectives[OBJ_LIFT2_ACTIVE] = true;
    }
    if (energy < 0 || energy > MAX_ENERGY ||
        time < 0 || time > MAX_TIME || score < 0)
        return false;
    if (playerFloor < 1 || playerFloor > NUM_FLOORS ||
        playerRoom < 1 || playerRoom > ROOMS_PER_FLOOR)
        return false;
    return true;
}

bool validateStartingRoom(
    int startFloor, int startRoom,
    const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1])
{
    if (startFloor < 1 || startFloor > NUM_FLOORS)
        return false;
    if (startRoom < 1 || startRoom > ROOMS_PER_FLOOR)
        return false;
    return true;
}

const char* getEntityName(char entitySymbol)
{
    switch (entitySymbol)
    {
        case ENT_MASTER_KEY: return "Master Key";
        case ENT_ENERGY_PACK: return "Energy Pack";
        case ENT_EMERGENCY_TOOLKIT: return "Emergency Toolkit";
        case ENT_CLUE_4: return "Clue 4";
        case ENT_FUSE: return "Fuse";
        case ENT_CAT: return "Cat";
        case ENT_CONTROL_PANEL: return "Control Panel";
        case ENT_LIFT_PASS: return "Lift Pass";
        case ENT_PASSENGER: return "Passenger";
        case ENT_MEDICAL_KIT: return "Medical Kit";
        case ENT_EMERGENCY_CARD: return "Emergency Card";
        case ENT_CLUE_2: return "Clue 2";
        case ENT_CLUE_7: return "Clue 7";
        case ENT_CABINET_TRAIN_KEY: return "Cabinet / Train Key";
        case ENT_CAT_CARRIER: return "Cat Carrier";
        case ENT_OBSERVATION_TOKEN: return "Observation Token";
        default: return "Unknown Entity";
    }
}

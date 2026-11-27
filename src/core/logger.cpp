
#include "../../include/logger.h"
#include "../../include/util.h"
#include"../../include/level.h"
#include <fstream>
using namespace std;

// Save game data
bool saveGameStateToFile(
    const char* filename,
    int playerFloor, int playerRoom,
    int energy, int time, int score,
    const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
    const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft,
    const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ofstream file(filename);

    if (!file) return false;

    file << playerFloor << endl;
    file << playerRoom << endl;
    file << energy << endl;
    file << time << endl;
    file << score << endl;
    file << inventoryCount << endl;

    for (int i = 0; i < inventoryCount; i++)
        file << inventory[i] << endl;

    for (int i = 0; i < NUM_OBJECTIVES; i++)
        file << objectives[i] << endl;

    file << emergencyTripsLeft << endl;

    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            file << entityCounts[f][r] << endl;

            for (int e = 0; e < entityCounts[f][r]; e++)
                file << roomEntities[f][r][e] << endl;
        }
    }

    file.close();
    return true;
}


// Load saved game data
bool loadGameStateFromFile(
    const char* filename,
    int& playerFloor, int& playerRoom,
    int& energy, int& time, int& score,
    char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
    bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
    char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);

    if (!file) return false;

    // Check player information
    if (!(file >> playerFloor >> playerRoom >> energy >> time >> score))
        return false;

    if (playerFloor < 1 || playerFloor > NUM_FLOORS ||
        playerRoom < 1 || playerRoom > ROOMS_PER_FLOOR ||
        energy < 0 || energy > MAX_ENERGY ||
        time < 0 || time > MAX_TIME)
        return false;

    // Check inventory
    if (!(file >> inventoryCount))
        return false;

    if (inventoryCount < 0 ||
        inventoryCount > MAX_INVENTORY_CAPACITY)
        return false;

    for (int i = 0; i < inventoryCount; i++)
    {
        if (!(file >> inventory[i]))
            return false;
    }

    // Load objectives
    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        int value;

        if (!(file >> value) || (value != 0 && value != 1))
            return false;

        objectives[i] = (value == 1);
    }

    // Check emergency trips
    if (!(file >> emergencyTripsLeft) || emergencyTripsLeft < 0)
        return false;

    // Load room entities
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            if (!(file >> entityCounts[f][r]))
                return false;

            if (entityCounts[f][r] < 0 ||
                entityCounts[f][r] > MAX_ENTITIES_PER_ROOM)
                return false;

            for (int e = 0; e < entityCounts[f][r]; e++)
            {
                if (!(file >> roomEntities[f][r][e]))
                    return false;
            }
        }
    }

    file.close();
    return true;
}


// Write game summary
void writeGameSummaryReport(
    const char* filename,
    int score, int remainingEnergy, int remainingTime,
    const bool objectives[NUM_OBJECTIVES], bool victory)
{
    ofstream file(filename);

    if (!file) return;

    file << "THE LAST TRAIN - GAME SUMMARY" << endl;
    file << "Final Score: " << score << endl;
    file << "Remaining Energy: " << remainingEnergy << endl;
    file << "Remaining Time: " << remainingTime << endl;

    file << endl << "Objectives:" << endl;

    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        file << "Objective " << i + 1 << ": ";

        if (objectives[i])
            file << "Completed" << endl;
        else
            file << "Not Completed" << endl;
    }

    if (victory)
        file << "Result: VICTORY" << endl;
    else
        file << "Result: DEFEAT" << endl;

    file.close();
}

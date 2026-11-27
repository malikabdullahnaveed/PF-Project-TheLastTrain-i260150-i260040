
#include "level.h"
#include "util.h"
#include <fstream>
#include <cstring>
#include<iostream>
using namespace std;
// Check whether a character represents an entity in the building
bool isEntitySymbol(char c)
{
    return c == '<' || c == ')' || c == '\\' ||
           c == '4' || c == '*' || c == '~' ||
           c == ';' || c == '(' || c == '!' ||
           c == '%' || c == '$' || c == '2' ||
           c == '7' || c == '^' || c == '@' ||
           c == '&';
}
// Read the building map from a file
bool loadBuildingMap(
    const char* filename,
    char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
    char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);

    // Check whether the file opened successfully
    if (!file)
    {
        return false;
    }

    // Initialize all arrays
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            entityCounts[f][r] = 0;

            for (int e = 0; e < MAX_ENTITIES_PER_ROOM; e++)
            {
                roomEntities[f][r][e] = '\0';
            }

            if (r < ROOMS_PER_FLOOR - 1)
            {
                horizontalConn[f][r] = '\0';
            }
        }
    }

    bool floorsLoaded[NUM_FLOORS] = {false};

    char line[MAX_LINE_LEN];
    char entityLine[MAX_LINE_LEN];

    // Read the building file one line at a time
    while (file.getline(line, MAX_LINE_LEN))
    {
        // Check whether this line contains a floor's room information
        bool floorRow = true;

        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            int position = 10 + (r * 12);

            if (line[position] != 'F' ||
                line[position + 2] != '-' ||
                line[position + 3] != 'R' ||
                line[position + 4] != ('1' + r))
            {
                floorRow = false;
                break;
            }
        }

        // Ignore lines that do not contain room labels
        if (!floorRow)
        {
            continue;
        }

        int floorNumber = line[11] - '0';

        // Check that the floor number is valid
        if (floorNumber < 1 || floorNumber > NUM_FLOORS)
        {
            return false;
        }

        int floorIndex = floorNumber - 1;

        // Do not load the same floor twice
        if (floorsLoaded[floorIndex])
        {
            return false;
        }

        // Read the next line, which contains room entities
        if (!file.getline(entityLine, MAX_LINE_LEN))
        {
            return false;
        }

        if (strlen(entityLine) <= 56)
        {
            return false;
        }

        // Read connections between neighboring rooms
        for (int r = 0; r < ROOMS_PER_FLOOR - 1; r++)
        {
            int connectionPosition = 20 + (r * 12);

            horizontalConn[floorIndex][r] = line[connectionPosition];
        }

        // Read entities inside each room
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            int startPosition = 9 + (r * 12);

            for (int i = 0; i < 12; i++)
            {
                char symbol = entityLine[startPosition + i];

                if (isEntitySymbol(symbol))
                {
                    int count = entityCounts[floorIndex][r];

                    if (count >= MAX_ENTITIES_PER_ROOM)
                    {
                        return false;
                    }

                    roomEntities[floorIndex][r][count] = symbol;
                    entityCounts[floorIndex][r]++;
                }
            }
        }

        floorsLoaded[floorIndex] = true;
    }

    // Make sure all five floors were found
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        if (!floorsLoaded[f])
        {
            return false;
        }
    }

    return true;
}


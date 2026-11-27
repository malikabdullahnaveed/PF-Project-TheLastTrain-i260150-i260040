#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments without structures
//  two rooms are directly connected on the same floor ?
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType)
{
    outConnType = CONN_NONE;

    if (fromF < 1 || fromF > NUM_FLOORS ||
        toF < 1 || toF > NUM_FLOORS ||
        fromR < 1 || fromR > ROOMS_PER_FLOOR ||
        toR < 1 || toR > ROOMS_PER_FLOOR)
    {
        return false;
    }

    
    if (fromF == toF && fromR == toR)
        return false;

    if (fromF == toF)
    {
        int difference = fromR - toR;
        if (difference != 1 && difference != -1)
            return false;

        int row = fromF - 1;
        int column;
        if (fromR < toR)
            column = fromR - 1;
        else
            column = toR - 1;

        outConnType = horizontalConn[row][column];
        if (outConnType == CONN_NONE || outConnType == CONN_SOLID_WALL)
        {
            outConnType = CONN_NONE;
            return false;
        }
        return true;
    }

    if (fromR == 1 && toR == 1)
    {
        if ((fromF == 1 && toF == 2) || (fromF == 2 && toF == 1) ||
            (fromF == 2 && toF == 3) || (fromF == 3 && toF == 2) ||
            (fromF == 4 && toF == 5) || (fromF == 5 && toF == 4))
        {
            outConnType = CONN_STAIRS;
            return true;
        }
    }

    if ((fromF == 1 && toF == 2) || (fromF == 2 && toF == 1))
    {
        if ((fromR == 2 || fromR == 3) && (toR == 2 || toR == 3))
        {
            outConnType = CONN_LIFT1;
            return true;
        }
    }

    if (fromR == 4 && toR == 4)
    {
        if ((fromF == 2 || fromF == 4 || fromF == 5) &&
            (toF == 2 || toF == 4 || toF == 5))
        {
            outConnType = CONN_LIFT2;
            return true;
        }
    }

    return false;
}

// the player is allowed to use a particular connection ?
bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational)
{
    //  the destination coordinates are valid ?
    if (targetFloor < 1 || targetFloor > NUM_FLOORS ||
        targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR)
    {
        return false;
    }

    // ordinary door and window does not require an item
    if (connType == CONN_DOOR)
    {
        return true;
    }

    //window 
    if (connType == CONN_WINDOW)
    {
        return true;
    }

    // locked door?
    if (connType == CONN_LOCKED_DOOR)
    {
        if (hasItem(inventory, inventoryCount, ENT_MASTER_KEY))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // Staircase Check
    
    if (connType == CONN_STAIRS)
    {
    if (targetRoom == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
    }
    // Lift 1 check
    if (connType == CONN_LIFT1)
    {
        if ((targetFloor == 1 || targetFloor == 2) &&
            (targetRoom == 2 || targetRoom == 3))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    //Lift 2 check ( can be operational and Not operational )
    if (connType == CONN_LIFT2)
    {
        if (targetRoom != 4 ||
            (targetFloor != 2 && targetFloor != 4 && targetFloor != 5))
        {
            return false;
        }

        // If Lift 2 is operational, the player needs a Lift Pass
        if (lift2Operational)
        {
            if (hasItem(inventory, inventoryCount, ENT_LIFT_PASS))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        // If Lift 2 is not operational, emergency access is required
        if (hasItem(inventory, inventoryCount, ENT_EMERGENCY_CARD) &&
            emergencyTripsLeft > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
// function 3
// Updates Energy and Time within their allowed limits
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime)
{
    energy = clampInt(energy + deltaEnergy, 0, MAX_ENERGY);

    time = clampInt(time + deltaTime, 0, MAX_TIME);
}
// function 4
// Updates the player's score
void applyScoreDelta(int& score, int deltaScore)
{
    score = score + deltaScore;
}
// funtion 5
// Checks whether an item exists in the inventory
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY],
             int inventoryCount, char itemSymbol)
{
    if (inventoryCount < 0 || inventoryCount > MAX_INVENTORY_CAPACITY)
        return false;

    if (inventory == nullptr || itemSymbol == ENT_NONE)
        return false;

    for (int i = 0; i < inventoryCount; i++)
    {
        if (inventory[i] == itemSymbol)
        {
            return true;
        }
    }

    return false;
}
// function 6
// Adds an item to the inventory
bool addItem(char inventory[MAX_INVENTORY_CAPACITY],
             int& inventoryCount, char itemSymbol)
{
    if (inventory == nullptr || itemSymbol == ENT_NONE)
        return false;

    if (inventoryCount < 0 || inventoryCount >= MAX_INVENTORY_CAPACITY)
        return false;

    inventory[inventoryCount] = itemSymbol;

    inventoryCount++;

    return true;
}
// function 6th
// Removes an item from the inventory
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY],
                int& inventoryCount, char itemSymbol)
{
    if (inventory == nullptr)
        return false;

    if (inventoryCount < 0 || inventoryCount > MAX_INVENTORY_CAPACITY)
        return false;

    for (int i = 0; i < inventoryCount; i++)
    {
        if (inventory[i] == itemSymbol)
        {
            // Shift remaining items one position to the left
            for (int j = i; j < inventoryCount - 1; j++)
            {
                inventory[j] = inventory[j + 1];
            }
            inventoryCount--;
            inventory[inventoryCount] = ENT_NONE;
            return true;
        }
    }

    return false;
}
#include "switches.h"
#include "util.h"
#include "physics.h"

// Door lock mechanics, lifts, and cabinet verification without structures

// Lift 1 connects Floor 1 and Floor 2
bool canUseLift1(int fromFloor, int toFloor)
{
    if (fromFloor == 1 && toFloor == 2)
    {
        return true;
    }

    if (fromFloor == 2 && toFloor == 1)
    {
        return true;
    }

    return false;
}

// Lift 2 connects Floors 2, 4 and 5
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft)
{
    if (fromFloor == toFloor)
    {
        return false;
    }

    if (fromFloor != 2 && fromFloor != 4 && fromFloor != 5)
    {
        return false;
    }

    if (toFloor != 2 && toFloor != 4 && toFloor != 5)
    {
        return false;
    }

    // After the repair the lift works normally
    if (lift2Operational)
    {
        return true;
    }

    // Before the repair only emergency trips can be used
    if (emergencyTripsLeft > 0)
    {
        return true;
    }

    return false;
}

// The locked door between F3-R1 and F3-R2 needs the Master Key
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount)
{
    if (hasItem(inventory, inventoryCount, ENT_MASTER_KEY))
    {
        return true;
    }

    return false;
}

// Put the fuse into the Control Panel (the room is checked by the simulator)
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score)
{
    // The fuse can only be installed once
    if (objectives[OBJ_FUSE_INSTALLED])
    {
        return false;
    }

    // The player must be carrying the fuse
    if (!hasItem(inventory, inventoryCount, ENT_FUSE))
    {
        return false;
    }

    removeItem(inventory, inventoryCount, ENT_FUSE);

    objectives[OBJ_FUSE_INSTALLED] = true;
    objectives[OBJ_LIFT2_ACTIVE] = true;
    applyScoreDelta(score, SCORE_INSTALL_FUSE);

    return true;
}

// The code is 427 and all three clue digits must be found first
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES])
{
    if (!objectives[OBJ_CLUE_4] || !objectives[OBJ_CLUE_2] || !objectives[OBJ_CLUE_7])
    {
        return false;
    }

    if (stringEquals(enteredCode, "427"))
    {
        return true;
    }

    return false;
}
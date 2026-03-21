#pragma once
#include "globals.h"

struct Ladybug
{
    float x, z;    // horizontal position (y always follows terrain)
    float yaw;     // facing direction in degrees
    float timer;   // frames until next random direction change
};

const int NUM_LADYBUGS = 8;
extern Ladybug ladybugs[NUM_LADYBUGS];

void initLadybugs();
void updateLadybugs();
void drawLadybug(float x, float y, float z, float yaw);
void drawAllLadybugs();

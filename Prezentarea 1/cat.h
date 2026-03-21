#pragma once
#include "globals.h"

struct Cat
{
    float angle;     // current angle on circular path (radians)
    float legPhase;  // walking cycle phase (radians)
};

extern Cat cat;

void initCat();
void updateCat();
void getCatTransform(float& x, float& z, float& yaw);
void drawCatAt(float x, float y, float z, float yaw);
void drawCat();
void drawKittenAt(float x, float y, float z, float yaw);
void drawKitten();
void drawOrangeKittenAt(float x, float y, float z, float yaw);
void drawOrangeKitten();
void drawWhiteKittenAt(float x, float y, float z, float yaw);
void drawWhiteKitten();
void drawGrayKittenAt(float x, float y, float z, float yaw);
void drawGrayKitten();

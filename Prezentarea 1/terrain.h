#pragma once

#include "globals.h"

float terrainHeight(float x, float z);
void computeNormal(float x, float z, float& nx, float& ny, float& nz);
void drawRelief();
void drawCircuit();
void drawFloor(float s);
void drawCeiling(float s, float h);
void drawWalls(float s, float h);

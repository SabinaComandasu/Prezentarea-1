#pragma once

#include "globals.h"

float degToRad(float a);
GLuint loadTexture(const char* filename);
void drawTexturedCylinder(float radius, float height, int slices);
void drawTexturedSphere(float radius, int stacks, int slices);

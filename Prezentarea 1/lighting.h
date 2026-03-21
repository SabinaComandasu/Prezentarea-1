#pragma once

#include "globals.h"

void setLighting();
void setMoonLight();
void setLampLight(GLenum lightId, float x, float z);
void drawMoon(float x, float y, float z, float radius);
void drawMoonRays(float x, float y, float z);
void drawMoonGlow(float x, float y, float z);
void drawLampPost(float x, float z);

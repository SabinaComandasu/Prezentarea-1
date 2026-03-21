#pragma once

#include <freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Bee (player) state
extern float beeX, beeY, beeZ;
extern float beeYaw;
extern float beeTargetYaw;

// Third-person camera
extern float camX, camY, camZ;
extern float yawAngle;
extern float pitchAngle;

extern const float camFollowDist;
extern const float camFollowHeight;

extern float moveSpeed;
extern float turnSpeed;
extern float pitchSpeed;

extern int windowWidth;
extern int windowHeight;

extern float mouseSensitivity;
extern bool ignoreWarp;
extern bool keySpace;
extern bool keyShift;
extern bool keyW;
extern bool keyA;
extern bool keyS;
extern bool keyD;

extern GLuint texGrass;
extern GLuint texSky;
extern GLuint texMountain;
extern GLuint texWater;
extern GLuint texWood;
extern GLuint texLeaves;
extern GLuint texMoon;

#include "globals.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Bee (player) state
float beeX = 0.0f, beeY = 8.0f, beeZ = 35.0f;
float beeYaw = 180.0f;
float beeTargetYaw = 180.0f;

// Third-person camera
float camX = 0.0f, camY = 8.0f, camZ = 35.0f;
float yawAngle = 180.0f;
float pitchAngle = -10.0f;

const float camFollowDist  = 10.0f;
const float camFollowHeight = 4.0f;

float moveSpeed = 1.0f;
float turnSpeed = 3.0f;
float pitchSpeed = 2.5f;

int windowWidth = 1200;
int windowHeight = 800;

float mouseSensitivity = 0.15f;
bool ignoreWarp = false;
bool keySpace = false;
bool keyShift = false;
bool keyW = false;
bool keyA = false;
bool keyS = false;
bool keyD = false;

GLuint texGrass = 0;
GLuint texSky = 0;
GLuint texMountain = 0;
GLuint texWater = 0;
GLuint texWood = 0;
GLuint texLeaves = 0;
GLuint texMoon = 0;

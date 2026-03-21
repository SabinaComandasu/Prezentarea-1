#include <freeglut.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

float degToRad(float a)
{
    return a * (float)M_PI / 180.0f;
}

GLuint loadTexture(const char* filename)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannels;

    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 3);

    if (!data)
    {
        printf("Nu s-a incarcat textura: %s\n", filename);
        return 0;
    }

    printf("Textura incarcata: %s | %d x %d | canale originale: %d\n",
        filename, width, height, nrChannels);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}

float terrainHeight(float x, float z)
{
    float d = sqrtf(x * x + z * z);

    float hill = 6.0f * expf(-(d * d) / 200.0f);
    float waves1 = 1.2f * sinf(0.28f * x) * cosf(0.28f * z);
    float waves2 = 0.5f * sinf(0.7f * x) * cosf(0.5f * z);

    return hill + waves1 + waves2;
}

void computeNormal(float x, float z, float& nx, float& ny, float& nz)
{
    float eps = 0.1f;

    float hL = terrainHeight(x - eps, z);
    float hR = terrainHeight(x + eps, z);
    float hD = terrainHeight(x, z - eps);
    float hU = terrainHeight(x, z + eps);

    nx = hL - hR;
    ny = 2.0f * eps;
    nz = hD - hU;

    float len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (len != 0.0f)
    {
        nx /= len;
        ny /= len;
        nz /= len;
    }
}



void drawCircuit()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texWater);

    glColor3f(1.0f, 1.0f, 1.0f);

    const float radius = 18.0f;
    const float halfWidth = 1.4f;
    const int segments = 240;
    const float lift = 0.19f;

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float t = 2.0f * (float)M_PI * i / segments;

        float c = cosf(t);
        float s = sinf(t);

        float outerX = (radius + halfWidth) * c;
        float outerZ = (radius + halfWidth) * s;

        float innerX = (radius - halfWidth) * c;
        float innerZ = (radius - halfWidth) * s;

        float outerY = terrainHeight(outerX, outerZ) + lift;
        float innerY = terrainHeight(innerX, innerZ) + lift;

        float u = (float)i / (float)segments * 10.0f;

        glNormal3f(0.0f, 1.0f, 0.0f);

        glTexCoord2f(u, 0.0f);
        glVertex3f(outerX, outerY, outerZ);

        glTexCoord2f(u, 1.0f);
        glVertex3f(innerX, innerY, innerZ);
    }
    glEnd();
}

void drawRelief()
{
    glBindTexture(GL_TEXTURE_2D, texGrass);
    glColor3f(0.75f, 0.75f, 0.75f);

    const int N = 160;
    const float size = 50.0f;
    const float step = (2.0f * size) / N;

    for (int i = 0; i < N; i++)
    {
        float z0 = -size + i * step;
        float z1 = z0 + step;

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= N; j++)
        {
            float x = -size + j * step;

            float y0 = terrainHeight(x, z0);
            float y1 = terrainHeight(x, z1);

            float nx, ny, nz;

            computeNormal(x, z0, nx, ny, nz);
            glNormal3f(nx, ny, nz);
            glTexCoord2f((x + size) / 5.0f, (z0 + size) / 5.0f);
            glVertex3f(x, y0, z0);

            computeNormal(x, z1, nx, ny, nz);
            glNormal3f(nx, ny, nz);
            glTexCoord2f((x + size) / 5.0f, (z1 + size) / 5.0f);
            glVertex3f(x, y1, z1);
        }
        glEnd();
    }
}

void drawFloor(float s)
{
    glBindTexture(GL_TEXTURE_2D, texGrass);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-s, 0.0f, -s);
    glTexCoord2f(12.0f, 0.0f);  glVertex3f(s, 0.0f, -s);
    glTexCoord2f(12.0f, 12.0f); glVertex3f(s, 0.0f, s);
    glTexCoord2f(0.0f, 12.0f);  glVertex3f(-s, 0.0f, s);
    glEnd();
}

void drawCeiling(float s, float h)
{
    glBindTexture(GL_TEXTURE_2D, texSky);
    glColor3f(0.35f, 0.35f, 0.40f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, h, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, h, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, h, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, h, s);
    glEnd();
}
void drawTexturedCylinder(float radius, float height, int slices)
{
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++)
    {
        float t = 2.0f * (float)M_PI * i / slices;
        float c = cosf(t);
        float s = sinf(t);

        float x = radius * c;
        float z = radius * s;

        glNormal3f(c, 0.0f, s);

        glTexCoord2f((float)i / slices * 4.0f, 0.0f);
        glVertex3f(x, 0.0f, z);

        glTexCoord2f((float)i / slices * 4.0f, 1.0f);
        glVertex3f(x, height, z);
    }
    glEnd();

    // capac jos
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= slices; i++)
    {
        float t = 2.0f * (float)M_PI * i / slices;
        float x = radius * cosf(t);
        float z = radius * sinf(t);

        glTexCoord2f(0.5f + 0.5f * cosf(t), 0.5f + 0.5f * sinf(t));
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // capac sus
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= slices; i++)
    {
        float t = 2.0f * (float)M_PI * i / slices;
        float x = radius * cosf(t);
        float z = radius * sinf(t);

        glTexCoord2f(0.5f + 0.5f * cosf(t), 0.5f + 0.5f * sinf(t));
        glVertex3f(x, height, z);
    }
    glEnd();
}
void drawTexturedSphere(float radius, int stacks, int slices)
{
    for (int i = 0; i < stacks; i++)
    {
        float phi1 = (float)M_PI * i / stacks - (float)M_PI / 2.0f;
        float phi2 = (float)M_PI * (i + 1) / stacks - (float)M_PI / 2.0f;

        float y1 = radius * sinf(phi1);
        float r1 = radius * cosf(phi1);

        float y2 = radius * sinf(phi2);
        float r2 = radius * cosf(phi2);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float theta = 2.0f * (float)M_PI * j / slices;
            float c = cosf(theta);
            float s = sinf(theta);

            float x1 = r1 * c;
            float z1 = r1 * s;

            float x2 = r2 * c;
            float z2 = r2 * s;

            glNormal3f(x1 / radius, y1 / radius, z1 / radius);
            glTexCoord2f((float)j / slices, (float)i / stacks);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2 / radius, y2 / radius, z2 / radius);
            glTexCoord2f((float)j / slices, (float)(i + 1) / stacks);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

void drawMoon(float x, float y, float z, float radius)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texMoon);

    GLfloat emission[] = { 0.35f, 0.35f, 0.40f, 1.0f };
    GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(x, y, z);
    drawTexturedSphere(radius, 24, 24);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

void drawMoonRays(float x, float y, float z)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
    glTranslatef(x, y, z);

    const float inner = 4.5f;   // start just outside moon
    const float outer = 16.0f;  // ray length
    const int rayCount = 8;

    for (int i = 0; i < rayCount; i++)
    {
        float angle = i * 360.0f / rayCount;

        glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLES);

        // bright near moon
        glColor4f(1.0f, 1.0f, 0.9f, 0.22f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-1.2f, -inner, 0.0f);
        glVertex3f(1.2f, -inner, 0.0f);

        // fade outward
        glColor4f(1.0f, 1.0f, 0.9f, 0.0f);
        glVertex3f(-4.0f, -outer, 0.0f);
        glVertex3f(4.0f, -outer, 0.0f);
        glVertex3f(0.0f, -inner, 0.0f);

        glEnd();

        glPopMatrix();
    }

    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawMoonGlow(float x, float y, float z)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 1.0f, 0.9f, 0.10f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(8.5f, 24, 24);
    glPopMatrix();

    glColor4f(1.0f, 1.0f, 0.9f, 0.05f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(11.0f, 24, 24);
    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}


void setMoonLight()
{
    GLfloat lightPos[] = { -45.0f, 31.8f, -45.0f, 1.0f };

    GLfloat ambient[] = { 0.08f, 0.08f, 0.12f, 1.0f };
    GLfloat diffuse[] = { 1.2f, 1.2f, 1.35f, 1.0f };
    GLfloat specular[] = { 1.3f, 1.3f, 1.45f, 1.0f };

    glEnable(GL_LIGHT4);
    glLightfv(GL_LIGHT4, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT4, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, specular);

    glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, 0.003f);
    glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.00005f);
}
void drawSakuraTree(float x, float z)
{
    float groundY = terrainHeight(x, z) - 0.35f;
    glPushMatrix();
    glTranslatef(x, groundY, z);

    // ---------------- TRUNCHI ----------------
    glBindTexture(GL_TEXTURE_2D, texWood);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    drawTexturedCylinder(1.2f, 10.0f, 32);
    glPopMatrix();

    // ---------------- RAMURI PRINCIPALE ----------------
    glBindTexture(GL_TEXTURE_2D, texWood);

    glPushMatrix();
    glTranslatef(0.0f, 7.5f, 0.0f);
    glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
    drawTexturedCylinder(0.45f, 4.5f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 7.8f, 0.0f);
    glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
    drawTexturedCylinder(0.45f, 4.0f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 8.2f, 0.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    drawTexturedCylinder(0.4f, 3.8f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 8.0f, 0.0f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawTexturedCylinder(0.4f, 3.8f, 20);
    glPopMatrix();

    // ---------------- COROANA ----------------
    glBindTexture(GL_TEXTURE_2D, texLeaves);

    glPushMatrix();
    glTranslatef(0.0f, 12.0f, 0.0f);
    drawTexturedSphere(3.8f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5f, 11.0f, 0.0f);
    drawTexturedSphere(2.6f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 11.2f, 0.3f);
    drawTexturedSphere(2.7f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 10.8f, 2.5f);
    drawTexturedSphere(2.5f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 10.6f, -2.5f);
    drawTexturedSphere(2.5f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.8f, 13.0f, 1.5f);
    drawTexturedSphere(2.1f, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, 13.2f, -1.2f);
    drawTexturedSphere(2.1f, 16, 16);
    glPopMatrix();

    glPopMatrix();
}



void drawPinkHouse(float x, float z)
{
    const float w = 7.5f;
    const float d = 5.8f;
    const float h = 4.2f;
    const float roofH = 2.6f;

    float groundY = terrainHeight(x, z) - 0.55f;

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glScalef(1.8f, 1.8f, 1.8f);

    glDisable(GL_TEXTURE_2D);

    float skirt = 0.65f;

    // ---------------- BASE / FOUNDATION ----------------
    // brighter front, darker back/sides for stronger visual shading
    glColor3f(0.78f, 0.63f, 0.71f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    glColor3f(0.54f, 0.45f, 0.50f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    glColor3f(0.63f, 0.52f, 0.58f);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    glColor3f(0.68f, 0.56f, 0.62f);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    // ---------------- WALLS ----------------
    glColor3f(0.96f, 0.76f, 0.86f); // front = brightest
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glEnd();

    glColor3f(0.66f, 0.54f, 0.62f); // back = darkest
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glEnd();

    glColor3f(0.78f, 0.63f, 0.72f); // left
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glEnd();

    glColor3f(0.86f, 0.68f, 0.78f); // right
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glEnd();

    // ---------------- DOOR ----------------
    glColor3f(0.42f, 0.20f, 0.12f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 2.3f, d / 2.0f + 0.02f);
    glVertex3f(-0.75f, 2.3f, d / 2.0f + 0.02f);
    glEnd();

    // small brighter strip on the door
    glColor3f(0.55f, 0.28f, 0.16f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.68f, 0.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.52f, 0.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.52f, 2.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.68f, 2.15f, d / 2.0f + 0.03f);
    glEnd();

    // ---------------- WINDOWS ----------------
    GLfloat windowEmission[] = { 0.18f, 0.18f, 0.22f, 1.0f };
    GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, windowEmission);
    glColor3f(0.78f, 0.88f, 0.95f);

    // front left
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-2.7f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(-1.5f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(-1.5f, 2.6f, d / 2.0f + 0.02f);
    glVertex3f(-2.7f, 2.6f, d / 2.0f + 0.02f);
    glEnd();

    // front right
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(2.7f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(2.7f, 2.6f, d / 2.0f + 0.02f);
    glVertex3f(1.5f, 2.6f, d / 2.0f + 0.02f);
    glEnd();

    // left window
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, -1.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, -1.0f);
    glEnd();

    // right window
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, 0.6f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, 0.6f);
    glEnd();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    // ---------------- ROOF ----------------
    // front roof
    glColor3f(0.88f, 0.38f, 0.62f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.78f, 0.62f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    // back roof
    glColor3f(0.56f, 0.24f, 0.40f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.78f, -0.62f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    // left roof
    glColor3f(0.68f, 0.30f, 0.48f);
    glBegin(GL_QUADS);
    glNormal3f(-0.78f, 0.62f, 0.0f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    // right roof
    glColor3f(0.80f, 0.34f, 0.56f);
    glBegin(GL_QUADS);
    glNormal3f(0.78f, 0.62f, 0.0f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    // side triangles
    glColor3f(0.90f, 0.74f, 0.82f);
    glBegin(GL_TRIANGLES);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h + roofH, 0.0f);
    glEnd();

    glColor3f(0.82f, 0.66f, 0.74f);
    glBegin(GL_TRIANGLES);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h + roofH, 0.0f);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void drawWalls(float s, float h)
{
    glBindTexture(GL_TEXTURE_2D, texMountain);

    // perete spate
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, 0.0f, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, 0.0f, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, h, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, h, -s);
    glEnd();

    // perete fata
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, 0.0f, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, 0.0f, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, h, s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, h, s);
    glEnd();

    // perete stanga
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, 0.0f, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, 0.0f, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, h, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, h, s);
    glEnd();

    // perete dreapta
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, 0.0f, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, 0.0f, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, h, s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, h, -s);
    glEnd();
}

void drawAxis()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
    glEnd();

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void setLampLight(GLenum lightId, float x, float z)
{
    GLfloat lightPos[] = { x, 6.0f, z, 1.0f };

    GLfloat ambient[] = { 0.05f, 0.05f, 0.04f, 1.0f };
    GLfloat diffuse[] = { 1.4f, 1.25f, 0.9f, 1.0f };
    GLfloat specular[] = { 1.4f, 1.25f, 0.9f, 1.0f };

    glEnable(lightId);

    glLightfv(lightId, GL_POSITION, lightPos);
    glLightfv(lightId, GL_AMBIENT, ambient);
    glLightfv(lightId, GL_DIFFUSE, diffuse);
    glLightfv(lightId, GL_SPECULAR, specular);

    glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(lightId, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.01f); // ADD HERE
}
void setLighting()
{
    GLfloat lightPos[] = { 15.0f, 25.0f, 15.0f, 1.0f };

    GLfloat ambient[] = { 0.005f, 0.005f, 0.01f, 1.0f };
    GLfloat diffuse[] = { 0.12f, 0.12f, 0.16f, 1.0f };
    GLfloat specular[] = { 0.18f, 0.18f, 0.22f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    setLampLight(GL_LIGHT1, -20.0f, 0.0f);
    setLampLight(GL_LIGHT2, 20.0f, 0.0f);
    setLampLight(GL_LIGHT3, 0.0f, 20.0f);
    setMoonLight();
}
void drawCrosshair()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    float cx = windowWidth * 0.5f;
    float cy = windowHeight * 0.5f;
    float size = 10.0f;
    float gap = 4.0f;

    glBegin(GL_LINES);
    // horizontal left
    glVertex2f(cx - size, cy);
    glVertex2f(cx - gap, cy);

    // horizontal right
    glVertex2f(cx + gap, cy);
    glVertex2f(cx + size, cy);

    // vertical bottom
    glVertex2f(cx, cy - size);
    glVertex2f(cx, cy - gap);

    // vertical top
    glVertex2f(cx, cy + gap);
    glVertex2f(cx, cy + size);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void resolveCollisions(float prevX, float prevY, float prevZ)
{
    const float beeRadius  = 1.2f;
    const float sceneLimit = 48.0f;
    const float sceneTop   = 33.0f;

    // Scene boundary walls
    if (beeX >  sceneLimit) beeX =  sceneLimit;
    if (beeX < -sceneLimit) beeX = -sceneLimit;
    if (beeZ >  sceneLimit) beeZ =  sceneLimit;
    if (beeZ < -sceneLimit) beeZ = -sceneLimit;
    if (beeY >  sceneTop)   beeY =  sceneTop;

    // Terrain floor
    float ground = terrainHeight(beeX, beeZ);
    float minY = ground + beeRadius + 0.5f;
    if (beeY < minY) beeY = minY;

    // Pink house — safety fallback (main collision handled per-axis in updateMovement)
    {
        float houseGround = terrainHeight(0.0f, 0.0f) - 0.55f;
        float houseTop    = houseGround + 4.2f + 2.6f;
        const float margin = 1.8f;
        float hx  = 3.75f + margin;
        float hz  = 2.9f  + margin;
        float hyt = houseTop    + margin;
        float hyb = houseGround - margin;

        if (fabsf(beeX) < hx && fabsf(beeZ) < hz && beeY < hyt && beeY > hyb)
        {
            float oPX = hx - beeX, oNX = beeX + hx;
            float oPZ = hz - beeZ, oNZ = beeZ + hz;
            float oPY = hyt - beeY;
            float m = oPX; int f = 0;
            if (oNX < m) { m = oNX; f = 1; }
            if (oPZ < m) { m = oPZ; f = 2; }
            if (oNZ < m) { m = oNZ; f = 3; }
            if (oPY < m) {          f = 4; }
            if      (f == 0) beeX =  hx;
            else if (f == 1) beeX = -hx;
            else if (f == 2) beeZ =  hz;
            else if (f == 3) beeZ = -hz;
            else             beeY =  hyt;
        }
    }

    // Lamp posts — cylinder collision for each pole
    {
        const float poleRadius = 0.30f + beeRadius; // 0.20 * 1.5 scale
        const float poleHeight = 10.2f;             // 6.8  * 1.5 scale
        float lampPositions[3][2] = { {-20.0f, 0.0f}, {20.0f, 0.0f}, {0.0f, 20.0f} };
        for (int i = 0; i < 3; i++)
        {
            float lx = lampPositions[i][0], lz = lampPositions[i][1];
            float lGround = terrainHeight(lx, lz) - 0.15f;
            float lTop    = lGround + poleHeight;

            if (beeY > lGround && beeY < lTop + beeRadius)
            {
                float dx = beeX - lx, dz = beeZ - lz;
                float dist = sqrtf(dx * dx + dz * dz);
                if (dist < poleRadius && dist > 0.001f)
                {
                    beeX = lx + (dx / dist) * poleRadius;
                    beeZ = lz + (dz / dist) * poleRadius;
                }
            }
        }
    }

    // Sakura trees — cylinder collision for trunk + sphere collision for canopy
    {
        const float trunkRadius = 1.2f + beeRadius;
        const float r = 34.0f;
        for (int i = 0; i < 9; i++)
        {
            float angle = i * 2.0f * (float)M_PI / 9.0f;
            float tx = cosf(angle) * r;
            float tz = sinf(angle) * r;
            float tGroundY = terrainHeight(tx, tz) - 0.35f;

            float dx = beeX - tx;
            float dz = beeZ - tz;
            float horizDist = sqrtf(dx * dx + dz * dz);
            if (horizDist < trunkRadius && horizDist > 0.001f)
            {
                beeX = tx + (dx / horizDist) * trunkRadius;
                beeZ = tz + (dz / horizDist) * trunkRadius;
            }

            struct { float ox, oy, oz, r; } clusters[] = {
                {  0.0f, 12.0f,  0.0f, 3.8f },
                { -2.5f, 11.0f,  0.0f, 2.6f },
                {  2.5f, 11.2f,  0.3f, 2.7f },
                {  0.0f, 10.8f,  2.5f, 2.5f },
                {  0.0f, 10.6f, -2.5f, 2.5f },
                { -1.8f, 13.0f,  1.5f, 2.1f },
                {  1.8f, 13.2f, -1.2f, 2.1f },
            };
            for (auto& c : clusters)
            {
                float cX = tx + c.ox, cY = tGroundY + c.oy, cZ = tz + c.oz;
                float minDist = c.r + beeRadius;
                float ex = beeX - cX, ey = beeY - cY, ez = beeZ - cZ;
                float d = sqrtf(ex * ex + ey * ey + ez * ez);
                if (d < minDist && d > 0.001f)
                {
                    beeX = cX + (ex / d) * minDist;
                    beeY = cY + (ey / d) * minDist;
                    beeZ = cZ + (ez / d) * minDist;
                }
            }
        }
    }
}

void updateMovement()
{
    float yawRad = degToRad(yawAngle);
    float fwdX = sinf(yawRad), fwdZ = cosf(yawRad);
    float rgtX = cosf(yawRad), rgtZ = -sinf(yawRad);

    float horizontalSpeed = 0.18f;
    float verticalSpeed   = 0.08f;

    float moveX = 0.0f, moveZ = 0.0f;
    if (keyW) { moveX += fwdX; moveZ += fwdZ; }
    if (keyS) { moveX -= fwdX; moveZ -= fwdZ; }
    if (keyA) { moveX += rgtX; moveZ += rgtZ; }
    if (keyD) { moveX -= rgtX; moveZ -= rgtZ; }

    // House keepout volume (computed once)
    float houseGround = terrainHeight(0.0f, 0.0f) - 0.55f;
    float houseTop    = houseGround + 4.2f + 2.6f;
    const float hMargin = 1.8f;
    float hx  = 3.75f + hMargin;
    float hz  = 2.9f  + hMargin;
    float hyt = houseTop    + hMargin;
    float hyb = houseGround - hMargin;
    auto insideHouse = [&](float x, float y, float z) -> bool {
        return fabsf(x) < hx && fabsf(z) < hz && y < hyt && y > hyb;
    };

    float prevX = beeX, prevY = beeY, prevZ = beeZ;

    float moveDist = sqrtf(moveX * moveX + moveZ * moveZ);
    if (moveDist > 0.001f)
    {
        beeTargetYaw = atan2f(moveX, moveZ) * 180.0f / (float)M_PI;
        float dX = (moveX / moveDist) * horizontalSpeed;
        float dZ = (moveZ / moveDist) * horizontalSpeed;

        // Apply each axis independently so bee slides along walls
        if (!insideHouse(beeX + dX, beeY, beeZ)) beeX += dX;
        if (!insideHouse(beeX, beeY, beeZ + dZ)) beeZ += dZ;
    }

    float diff = beeTargetYaw - beeYaw;
    while (diff >  180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;
    beeYaw += diff * 0.15f;

    if (keySpace) beeY += verticalSpeed;
    if (keyShift)
    {
        float tryY = beeY - verticalSpeed;
        // Block descending into house roof
        if (!insideHouse(beeX, tryY, beeZ) && tryY >= 2.0f) beeY = tryY;
        else if (tryY < 2.0f) beeY = 2.0f;
    }

    resolveCollisions(prevX, prevY, prevZ);

    // Camera orbits behind the camera-facing direction (yawAngle), not beeYaw
    float pitchRad  = degToRad(pitchAngle);
    float camYawRad = degToRad(yawAngle);
    camX = beeX - sinf(camYawRad) * cosf(pitchRad) * camFollowDist;
    camY = beeY - sinf(pitchRad)  * camFollowDist + camFollowHeight;
    camZ = beeZ - cosf(camYawRad) * cosf(pitchRad) * camFollowDist;
}

void drawBee(float x, float y, float z, float yawDeg)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yawDeg + 180.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(pitchAngle, 1.0f, 0.0f, 0.0f);

    glDisable(GL_TEXTURE_2D);

    // --- Body (abdomen) - elongated sphere, yellow with black stripes ---
    // Base yellow body
    glColor3f(1.0f, 0.85f, 0.0f);
    glPushMatrix();
    glScalef(0.5f, 0.45f, 0.85f);
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, 1.0f, 16, 12);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Black stripe 1
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.2f);
    glScalef(0.51f, 0.46f, 0.18f);
    q = gluNewQuadric();
    gluSphere(q, 1.0f, 16, 8);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Black stripe 2
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.2f);
    glScalef(0.51f, 0.46f, 0.18f);
    q = gluNewQuadric();
    gluSphere(q, 1.0f, 16, 8);
    gluDeleteQuadric(q);
    glPopMatrix();

    // --- Head ---
    glColor3f(0.1f, 0.08f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -0.95f);
    glScalef(0.38f, 0.36f, 0.35f);
    q = gluNewQuadric();
    gluSphere(q, 1.0f, 14, 10);
    gluDeleteQuadric(q);
    glPopMatrix();

    // --- Eyes (small white spheres on head) ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(0.2f, 0.18f, -1.18f);
    q = gluNewQuadric();
    gluSphere(q, 0.1f, 8, 6);
    gluDeleteQuadric(q);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.2f, 0.18f, -1.18f);
    q = gluNewQuadric();
    gluSphere(q, 0.1f, 8, 6);
    gluDeleteQuadric(q);
    glPopMatrix();

    // --- Stinger ---
    glColor3f(0.15f, 0.1f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.9f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    q = gluNewQuadric();
    gluCylinder(q, 0.06f, 0.0f, 0.35f, 8, 1);
    gluDeleteQuadric(q);
    glPopMatrix();

    // --- Antennae ---
    glColor3f(0.1f, 0.08f, 0.0f);
    // Left antenna
    glPushMatrix();
    glTranslatef(-0.15f, 0.3f, -1.1f);
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-40.0f, 1.0f, 0.0f, 0.0f);
    q = gluNewQuadric();
    gluCylinder(q, 0.03f, 0.02f, 0.45f, 6, 1);
    gluDeleteQuadric(q);
    // tip
    glTranslatef(0.0f, 0.0f, 0.45f);
    q = gluNewQuadric();
    gluSphere(q, 0.06f, 6, 5);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Right antenna
    glPushMatrix();
    glTranslatef(0.15f, 0.3f, -1.1f);
    glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-40.0f, 1.0f, 0.0f, 0.0f);
    q = gluNewQuadric();
    gluCylinder(q, 0.03f, 0.02f, 0.45f, 6, 1);
    gluDeleteQuadric(q);
    glTranslatef(0.0f, 0.0f, 0.45f);
    q = gluNewQuadric();
    gluSphere(q, 0.06f, 6, 5);
    gluDeleteQuadric(q);
    glPopMatrix();

    // --- Wings (semi-transparent flat quads) ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.8f, 0.9f, 1.0f, 0.55f);
    glDisable(GL_CULL_FACE);

    // Left wing
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-0.55f, 0.3f, -0.1f);
    glVertex3f(-0.55f, 0.3f, -0.6f);
    glVertex3f(-1.3f,  0.55f, -0.55f);
    glVertex3f(-1.5f,  0.5f,  -0.1f);
    glVertex3f(-1.2f,  0.45f,  0.25f);
    glVertex3f(-0.55f, 0.3f,   0.3f);
    glEnd();

    // Right wing
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.55f, 0.3f, -0.1f);
    glVertex3f(0.55f, 0.3f, -0.6f);
    glVertex3f(1.3f,  0.55f, -0.55f);
    glVertex3f(1.5f,  0.5f,  -0.1f);
    glVertex3f(1.2f,  0.45f,  0.25f);
    glVertex3f(0.55f, 0.3f,   0.3f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawLampPost(float x, float z)
{
    float groundY = terrainHeight(x, z) - 0.15f;

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glScalef(1.5f, 1.5f, 1.5f);   // bigger lamp posts

    glDisable(GL_TEXTURE_2D);

    // stalp principal
    glColor3f(0.12f, 0.12f, 0.12f);
    glPushMatrix();
    drawTexturedCylinder(0.20f, 6.8f, 20);
    glPopMatrix();

    // brat orizontal
    glPushMatrix();
    glTranslatef(0.0f, 6.2f, 0.0f);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);   // face cilindrul orizontal pe axa X
    drawTexturedCylinder(0.08f, 1.2f, 12);
    glPopMatrix();

    // suport vertical mic pentru bec
    glPushMatrix();
    glTranslatef(-1.2f, 6.2f, 0.0f);      // capatul bratului
    drawTexturedCylinder(0.05f, 0.45f, 10);
    glPopMatrix();

    // becul
    GLfloat emission[] = { 1.0f, 0.95f, 0.65f, 1.0f };
    GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glColor3f(1.0f, 0.98f, 0.80f);

    glPushMatrix();
    glTranslatef(-1.2f, 5.78f, 0.0f);
    drawTexturedSphere(0.28f, 14, 14);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPopMatrix();
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    updateMovement();

    // Third-person: camera always looks at the bee
    gluLookAt(
        camX, camY, camZ,
        beeX, beeY, beeZ,
        0.0f, 1.0f, 0.0f
    );

    setLighting();

    const float sceneHalf = 50.0f;
    const float sceneHeight = 35.0f;

    glColor3f(1.0f, 1.0f, 1.0f);

    drawFloor(sceneHalf);
    drawCeiling(sceneHalf, sceneHeight);
    drawWalls(sceneHalf, sceneHeight);
    drawMoonGlow(-45.0f, 31.8f, -45.0f);

    drawMoon(-45.0f, 31.8f, -45.0f, 9.0f); 
    drawMoonRays(-45.0f, 31.8f, -44.8f);
    drawRelief();
    drawCircuit();
    drawPinkHouse(0.0f, 0.0f);
    float r = 34.0f;  

    for (int i = 0; i < 9; i++)
    {
        float angle = i * 2.0f * M_PI / 9.0f;

        float x = cos(angle) * r;
        float z = sin(angle) * r;
        drawSakuraTree(x, z);
    }
    drawBee(beeX, beeY, beeZ, beeYaw);
    // drawAxis();
    drawLampPost(-20.0f, 0.0f);
    drawLampPost(20.0f, 0.0f);
    drawLampPost(0.0f, 20.0f);

    drawCrosshair();
    glutSwapBuffers();
    glutPostRedisplay();
}


void reshape(int w, int h)
{
    if (h == 0) h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 300.0);

    glMatrixMode(GL_MODELVIEW);
}
void specialKeys(int key, int, int)
{
    if (key == GLUT_KEY_SHIFT_L)
        keyShift = true;
}
void specialKeysUp(int key, int, int)
{
    if (key == GLUT_KEY_SHIFT_L)
        keyShift = false;
}


void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
    case 27:
        std::exit(0);
        break;

    case 'w':
    case 'W':
        keyW = true;
        break;

    case 's':
    case 'S':
        keyS = true;
        break;

    case 'a':
    case 'A':
        keyA = true;
        break;

    case 'd':
    case 'D':
        keyD = true;
        break;

    case 'q':
    case 'Q':
        yawAngle -= turnSpeed;
        break;

    case 'e':
    case 'E':
        yawAngle += turnSpeed;
        break;

    case ' ':
        keySpace = true;
        break;

    case 't':
    case 'T':
        pitchAngle += pitchSpeed;
        if (pitchAngle > 89.0f) pitchAngle = 89.0f;
        break;

    case 'g':
    case 'G':
        pitchAngle -= pitchSpeed;
        if (pitchAngle < -89.0f) pitchAngle = -89.0f;
        break;
    }

    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int, int)
{
    switch (key)
    {
    case 'w':
    case 'W':
        keyW = false;
        break;

    case 's':
    case 'S':
        keyS = false;
        break;

    case 'a':
    case 'A':
        keyA = false;
        break;

    case 'd':
    case 'D':
        keyD = false;
        break;

    case ' ':
        keySpace = false;
        break;
    }
}

void mouseLook(int x, int y)
{
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    // ignore the fake event generated by glutWarpPointer
    if (ignoreWarp)
    {
        ignoreWarp = false;
        return;
    }

    int dx = x - centerX;
    int dy = y - centerY;

    yawAngle -= dx * mouseSensitivity;
    pitchAngle -= dy * mouseSensitivity;

    if (pitchAngle > 89.0f) pitchAngle = 89.0f;
    if (pitchAngle < -89.0f) pitchAngle = -89.0f;

    ignoreWarp = true;
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}

void init()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_NORMALIZE);   


    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    texGrass = loadTexture("grass.jpg");
    texSky = loadTexture("sky.jpg");
    texMountain = loadTexture("tree.jpg");
    texWater = loadTexture("water.jpg");
    texWood = loadTexture("wood.jpg");
    texLeaves = loadTexture("leaves.jpg");
    texMoon = loadTexture("moon.jpg");

    if (texGrass == 0 || texSky == 0 || texMountain == 0 || texWater == 0 ||
        texWood == 0 || texLeaves == 0 || texMoon == 0)
    {
        printf("Una sau mai multe texturi nu s-au incarcat.\n");
    }

    glutSetCursor(GLUT_CURSOR_NONE);
    ignoreWarp = true;
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("P1 - Scena intr-un cub");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);

    glutPassiveMotionFunc(mouseLook);
    glutMotionFunc(mouseLook);

    glutMainLoop();
    return 0;
}
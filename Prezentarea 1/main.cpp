#include <freeglut.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float camX = 0.0f, camY = 8.0f, camZ = 35.0f;
float yawAngle = 180.0f;
float pitchAngle = -5.0f;

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

    const int N = 120;
    const float size = 40.0f;
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

    glDisable(GL_TEXTURE_2D);

    glColor3f(0.82f, 0.66f, 0.74f);

    float skirt = 0.65f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    // back
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    // left
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    // right
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    // ---------------- WALLS ----------------
    glColor3f(1.0f, 0.75f, 0.86f);

    // front
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glEnd();

    // back
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glEnd();

    // left
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glEnd();

    // right
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glEnd();

    // ---------------- DOOR ----------------
    glColor3f(0.60f, 0.30f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 2.3f, d / 2.0f + 0.02f);
    glVertex3f(-0.75f, 2.3f, d / 2.0f + 0.02f);
    glEnd();

    // ---------------- WINDOWS ----------------
    glColor3f(0.85f, 0.95f, 1.0f);

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

    // side windows
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, -1.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, -1.0f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, 0.6f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, 0.6f);
    glEnd();

    // ---------------- ROOF ----------------
    glColor3f(0.95f, 0.45f, 0.70f);

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.6f, 0.8f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.6f, -0.8f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-0.8f, 0.6f, 0.0f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.8f, 0.6f, 0.0f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    glColor3f(1.0f, 0.82f, 0.90f);

    glBegin(GL_TRIANGLES);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h + roofH, 0.0f);
    glEnd();

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

void setLighting()
{
    GLfloat lightPos[] = { 15.0f, 25.0f, 15.0f, 1.0f };
    GLfloat ambient[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    GLfloat specular[] = { 0.6f,  0.6f,  0.6f,  1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
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

void updateMovement()
{
    float yawRad = degToRad(yawAngle);

    float dirX = sinf(yawRad);
    float dirZ = cosf(yawRad);

    float rightX = cosf(yawRad);
    float rightZ = -sinf(yawRad);

    float verticalSpeed = 0.08f;   // slower than 0.25
    float horizontalSpeed = 0.18f; // smooth continuous WASD

    if (keyW)
    {
        camX += dirX * horizontalSpeed;
        camZ += dirZ * horizontalSpeed;
    }

    if (keyS)
    {
        camX -= dirX * horizontalSpeed;
        camZ -= dirZ * horizontalSpeed;
    }

    if (keyA)
    {
        camX += rightX * horizontalSpeed;
        camZ += rightZ * horizontalSpeed;
    }

    if (keyD)
    {
        camX -= rightX * horizontalSpeed;
        camZ -= rightZ * horizontalSpeed;
    }

    if (keySpace)
        camY += verticalSpeed;

    if (keyShift)
    {
        camY -= verticalSpeed;
        if (camY < 2.0f) camY = 2.0f;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    updateMovement();
    float yawRad = degToRad(yawAngle);
    float pitchRad = degToRad(pitchAngle);

    float dirX = cosf(pitchRad) * sinf(yawRad);
    float dirY = sinf(pitchRad);
    float dirZ = cosf(pitchRad) * cosf(yawRad);

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f
    );

    setLighting();

    const float sceneHalf = 50.0f;
    const float sceneHeight = 35.0f;

    glColor3f(1.0f, 1.0f, 1.0f);

    drawFloor(sceneHalf);
    drawCeiling(sceneHalf, sceneHeight);
    drawWalls(sceneHalf, sceneHeight);
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
    // drawAxis();
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
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    texGrass = loadTexture("grass.jpg");
    texSky = loadTexture("sky.jpg");
    texMountain = loadTexture("tree.jpg");
    texWater = loadTexture("water.jpg");
    texWood = loadTexture("wood.jpg");
    texLeaves = loadTexture("leaves.jpg");

    if (texGrass == 0 || texSky == 0 || texMountain == 0 || texWater == 0 || texWood == 0 || texLeaves == 0)
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
#include "terrain.h"

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

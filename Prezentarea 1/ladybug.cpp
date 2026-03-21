#include "ladybug.h"
#include "terrain.h"
#include "utils.h"

Ladybug ladybugs[NUM_LADYBUGS];

static const float LADYBUG_SPEED  = 0.04f;
static const float LADYBUG_RADIUS = 0.7f;
static const float SCENE_LIMIT    = 45.0f;

// House keepout (same margin as bee collision)
static bool nearHouse(float x, float z)
{
    return fabsf(x) < 7.0f && fabsf(z) < 6.0f;
}

void initLadybugs()
{
    // Spread starting positions across the map, avoiding house and scene edges
    float startPositions[NUM_LADYBUGS][2] = {
        {  18.0f, -12.0f },
        { -22.0f,   8.0f },
        {   8.0f,  28.0f },
        { -15.0f, -25.0f },
        {  30.0f,  15.0f },
        { -30.0f, -10.0f },
        {  12.0f, -35.0f },
        { -10.0f,  35.0f },
    };

    for (int i = 0; i < NUM_LADYBUGS; i++)
    {
        ladybugs[i].x     = startPositions[i][0];
        ladybugs[i].z     = startPositions[i][1];
        ladybugs[i].yaw   = (float)(rand() % 360);
        ladybugs[i].timer = (float)(60 + rand() % 120);
    }
}

void updateLadybugs()
{
    for (int i = 0; i < NUM_LADYBUGS; i++)
    {
        Ladybug& lb = ladybugs[i];

        // Count down direction timer
        lb.timer -= 1.0f;
        if (lb.timer <= 0.0f)
        {
            lb.yaw   = (float)(rand() % 360);
            lb.timer = (float)(80 + rand() % 140);
        }

        // Move forward along current yaw
        float rad = lb.yaw * (float)M_PI / 180.0f;
        float nx  = lb.x + sinf(rad) * LADYBUG_SPEED;
        float nz  = lb.z + cosf(rad) * LADYBUG_SPEED;

        // Bounce off scene boundaries and house
        if (nx > SCENE_LIMIT || nx < -SCENE_LIMIT || nz > SCENE_LIMIT || nz < -SCENE_LIMIT || nearHouse(nx, nz))
        {
            lb.yaw   = (float)(rand() % 360);
            lb.timer = (float)(80 + rand() % 140);
        }
        else
        {
            lb.x = nx;
            lb.z = nz;
        }

        // Ladybug-to-ladybug separation
        for (int j = 0; j < NUM_LADYBUGS; j++)
        {
            if (i == j) continue;
            float dx = lb.x - ladybugs[j].x;
            float dz = lb.z - ladybugs[j].z;
            float dist = sqrtf(dx * dx + dz * dz);
            if (dist < LADYBUG_RADIUS * 2.0f && dist > 0.001f)
            {
                lb.x = ladybugs[j].x + (dx / dist) * LADYBUG_RADIUS * 2.0f;
                lb.z = ladybugs[j].z + (dz / dist) * LADYBUG_RADIUS * 2.0f;
                lb.yaw = (float)(rand() % 360);
            }
        }
    }
}

void drawLadybug(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);

    glDisable(GL_TEXTURE_2D);

    GLUquadric* q = gluNewQuadric();

    // --- Body (red ellipsoid) ---
    glColor3f(0.85f, 0.05f, 0.05f);
    glPushMatrix();
    glScalef(0.38f, 0.25f, 0.5f);
    gluSphere(q, 1.0f, 14, 10);
    glPopMatrix();

    // --- Shell dividing line (black strip down the middle) ---
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.03f, 0.03f, 1.0f, 6, 1);
    glPopMatrix();

    // --- Black spots (3 on body) ---
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix(); glTranslatef( 0.18f, 0.26f, -0.1f); gluSphere(q, 0.09f, 8, 6); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.18f, 0.26f, -0.1f); gluSphere(q, 0.09f, 8, 6); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f,  0.26f,  0.2f); gluSphere(q, 0.08f, 8, 6); glPopMatrix();

    // --- Head (black sphere) ---
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, -0.55f);
    glScalef(0.28f, 0.22f, 0.22f);
    gluSphere(q, 1.0f, 12, 8);
    glPopMatrix();

    // --- Eyes (tiny white spheres) ---
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix(); glTranslatef( 0.12f, 0.13f, -0.68f); gluSphere(q, 0.055f, 6, 5); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.12f, 0.13f, -0.68f); gluSphere(q, 0.055f, 6, 5); glPopMatrix();

    // --- Antennae ---
    glColor3f(0.08f, 0.08f, 0.08f);
    // Left
    glPushMatrix();
    glTranslatef(-0.08f, 0.18f, -0.65f);
    glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.02f, 0.01f, 0.3f, 5, 1);
    glTranslatef(0.0f, 0.0f, 0.3f);
    gluSphere(q, 0.04f, 5, 4);
    glPopMatrix();
    // Right
    glPushMatrix();
    glTranslatef( 0.08f, 0.18f, -0.65f);
    glRotatef( 25.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.02f, 0.01f, 0.3f, 5, 1);
    glTranslatef(0.0f, 0.0f, 0.3f);
    gluSphere(q, 0.04f, 5, 4);
    glPopMatrix();

    // --- Legs (6 total, 3 per side) ---
    glColor3f(0.08f, 0.08f, 0.08f);
    float legZOffsets[3] = { -0.25f, 0.0f, 0.25f };
    for (int i = 0; i < 3; i++)
    {
        // Left leg
        glPushMatrix();
        glTranslatef(-0.38f, 0.02f, legZOffsets[i]);
        glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        gluCylinder(q, 0.025f, 0.015f, 0.32f, 5, 1);
        glPopMatrix();
        // Right leg
        glPushMatrix();
        glTranslatef( 0.38f, 0.02f, legZOffsets[i]);
        glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        gluCylinder(q, 0.025f, 0.015f, 0.32f, 5, 1);
        glPopMatrix();
    }

    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawAllLadybugs()
{
    for (int i = 0; i < NUM_LADYBUGS; i++)
    {
        float ground = terrainHeight(ladybugs[i].x, ladybugs[i].z);
        if (ground < 0.0f) ground = 0.0f;
        drawLadybug(ladybugs[i].x, ground + 0.25f, ladybugs[i].z, ladybugs[i].yaw);
    }
}

#include "cat.h"
#include "terrain.h"
#include "utils.h"

Cat cat;

static const float CAT_CIRCLE_RADIUS     = 28.0f;
static const float CAT_ANGLE_SPEED       = 0.002f;
static const float CAT_LEG_SPEED         = 0.13f;
static const float KITTEN_SCALE          = 0.42f;   // tuxedo
static const float KITTEN_FOLLOW_OFFSET  = 0.32f;   // radians behind calico (all kittens)
static const float KITTEN2_SCALE         = 0.47f;   // orange
static const float KITTEN3_SCALE         = 0.44f;   // white
static const float KITTEN4_SCALE         = 0.40f;   // gray
static const float KITTEN2_FOLLOW_OFFSET = 0.62f;   // radians behind calico

void initCat()
{
    cat.angle    = 0.0f;
    cat.legPhase = 0.0f;
}

void updateCat()
{
    cat.angle    += CAT_ANGLE_SPEED;
    cat.legPhase += CAT_LEG_SPEED;
    if (cat.angle    > 2.0f * (float)M_PI) cat.angle    -= 2.0f * (float)M_PI;
    if (cat.legPhase > 2.0f * (float)M_PI) cat.legPhase -= 2.0f * (float)M_PI;
}

void getCatTransform(float& x, float& z, float& yaw)
{
    x   = cosf(cat.angle) * CAT_CIRCLE_RADIUS;
    z   = sinf(cat.angle) * CAT_CIRCLE_RADIUS;
    yaw = atan2f(-sinf(cat.angle), cosf(cat.angle)) * 180.0f / (float)M_PI;
}

// -------------------------------------------------------------------------
// Color helpers  (unit-sphere coords: z>0=back, z<0=front, x>0=right, y>0=top)
// -------------------------------------------------------------------------
static void catBodyColor(float x, float y, float z)
{
    float oField = y + 0.5f * z + 0.25f * x - 0.05f
                   + 0.08f * sinf(x * 5.0f + z * 3.0f);
    bool isOrange = oField > 0.20f;

    float bField = -x - 0.08f * z + 0.10f
                   + 0.07f * cosf(y * 4.0f + z * 2.5f);
    bool isBlack = bField > 0.18f && y > -0.40f && y < 0.58f;

    if      (isBlack)  glColor3f(0.12f, 0.12f, 0.12f);
    else if (isOrange) glColor3f(0.88f, 0.48f, 0.10f);
    else               glColor3f(0.95f, 0.93f, 0.90f);
}

static void catHeadColor(float x, float y, float z)
{
    (void)z;
    bool isBlack = (x > 0.25f) && (y > 0.30f);
    if (isBlack) glColor3f(0.12f, 0.12f, 0.12f);
    else         glColor3f(0.95f, 0.93f, 0.90f);
}

static void catWhiteColor(float x, float y, float z)
{
    (void)x; (void)y; (void)z;
    glColor3f(0.97f, 0.95f, 0.92f);
}

static void catBlackColor(float x, float y, float z)
{
    (void)x; (void)y; (void)z;
    glColor3f(0.10f, 0.10f, 0.10f);
}

// -------------------------------------------------------------------------
// Fluffy ellipsoid: vertices are pushed outward by a high-frequency
// sinusoidal factor, creating a bumpy, fur-like silhouette.
// -------------------------------------------------------------------------
typedef void (*ColorFn)(float x, float y, float z);

static void drawFluffyEllipsoid(float sx, float sy, float sz,
                                float fluff, ColorFn fn,
                                int lats, int lons)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    for (int i = 0; i < lats; i++)
    {
        float a0 = -(float)M_PI * 0.5f + i       * (float)M_PI / lats;
        float a1 = -(float)M_PI * 0.5f + (i + 1) * (float)M_PI / lats;
        float y0 = sinf(a0), r0 = cosf(a0);
        float y1 = sinf(a1), r1 = cosf(a1);
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= lons; j++)
        {
            float lon = j * 2.0f * (float)M_PI / lons;
            float cx = cosf(lon), cz = sinf(lon);

            // High-frequency radial perturbation — creates fur tufts
            float s1 = 1.0f + fluff * fabsf(sinf(lon * 7.0f) * sinf(a1 * 6.0f));
            float s0 = 1.0f + fluff * fabsf(sinf(lon * 7.0f) * sinf(a0 * 6.0f));

            fn(cx * r1, y1, cz * r1);
            glNormal3f(cx * r1, y1, cz * r1);
            glVertex3f(cx * r1 * s1, y1 * s1, cz * r1 * s1);

            fn(cx * r0, y0, cz * r0);
            glNormal3f(cx * r0, y0, cz * r0);
            glVertex3f(cx * r0 * s0, y0 * s0, cz * r0 * s0);
        }
        glEnd();
    }
    glPopMatrix();
}

void drawCatAt(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);

    glDisable(GL_TEXTURE_2D);

    GLUquadric* q = gluNewQuadric();

    // --- Body — fluffy calico ellipsoid ---
    drawFluffyEllipsoid(0.58f, 0.43f, 0.95f, 0.13f, catBodyColor, 20, 26);

    // --- Chest ruff (fluffy white patch at front-bottom of neck) ---
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, -0.82f);
    drawFluffyEllipsoid(0.32f, 0.28f, 0.18f, 0.18f, catWhiteColor, 10, 14);
    glPopMatrix();

    // --- Head — fluffy ---
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, -1.02f);
    drawFluffyEllipsoid(0.42f, 0.40f, 0.40f, 0.09f, catHeadColor, 14, 18);
    glPopMatrix();

    // --- Cheek puffs (fluffy white) ---
    glPushMatrix();
    glTranslatef(-0.30f, 0.14f, -1.10f);
    drawFluffyEllipsoid(0.20f, 0.17f, 0.15f, 0.18f, catWhiteColor, 8, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.30f, 0.14f, -1.10f);
    drawFluffyEllipsoid(0.20f, 0.17f, 0.15f, 0.18f, catWhiteColor, 8, 12);
    glPopMatrix();

    // --- Left ear ---
    glColor3f(0.95f, 0.75f, 0.75f);
    glPushMatrix();
    glTranslatef(-0.22f, 0.57f, -0.98f);
    glRotatef(-18.0f, 0.0f, 0.0f, 1.0f);
    glRotatef( 12.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.11f, 0.01f, 0.26f, 6, 1);
    glPopMatrix();

    // --- Right ear ---
    glPushMatrix();
    glTranslatef( 0.22f, 0.57f, -0.98f);
    glRotatef( 18.0f, 0.0f, 0.0f, 1.0f);
    glRotatef( 12.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.11f, 0.01f, 0.26f, 6, 1);
    glPopMatrix();

    // --- Eyes (green) ---
    glColor3f(0.20f, 0.75f, 0.25f);
    glPushMatrix(); glTranslatef(-0.15f, 0.24f, -1.37f); gluSphere(q, 0.075f, 8, 6); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.15f, 0.24f, -1.37f); gluSphere(q, 0.075f, 8, 6); glPopMatrix();

    // --- Pupils ---
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix(); glTranslatef(-0.15f, 0.24f, -1.44f); gluSphere(q, 0.040f, 6, 4); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.15f, 0.24f, -1.44f); gluSphere(q, 0.040f, 6, 4); glPopMatrix();

    // --- Nose ---
    glColor3f(0.90f, 0.50f, 0.55f);
    glPushMatrix(); glTranslatef(0.0f, 0.13f, -1.40f); gluSphere(q, 0.045f, 6, 5); glPopMatrix();

    // --- Whiskers ---
    glColor3f(0.90f, 0.90f, 0.90f);
    glPushMatrix(); glTranslatef(-0.10f, 0.12f, -1.40f); glRotatef( 8.0f,0,1,0); glRotatef(90,0,1,0); gluCylinder(q,0.012f,0.006f,0.42f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.10f, 0.10f, -1.40f); glRotatef(-5.0f,0,1,0); glRotatef(90,0,1,0); gluCylinder(q,0.012f,0.006f,0.42f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.10f, 0.12f, -1.40f); glRotatef(-8.0f,0,1,0); glRotatef(-90,0,1,0); gluCylinder(q,0.012f,0.006f,0.42f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.10f, 0.10f, -1.40f); glRotatef( 5.0f,0,1,0); glRotatef(-90,0,1,0); gluCylinder(q,0.012f,0.006f,0.42f,4,1); glPopMatrix();

    // --- Bushy tail — black with white fluffy tip ---
    glColor3f(0.10f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.94f);
    glRotatef(-50.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.22f, 0.14f, 0.50f, 10, 1);
    // Black fluffy spheres along the tail length (all but the last)
    for (int i = 0; i <= 4; i++)
    {
        float t   = (float)i / 5.0f;
        float rad = 0.20f - t * 0.06f;
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.50f * t);
        drawFluffyEllipsoid(rad, rad, rad, 0.22f, catBlackColor, 7, 10);
        glPopMatrix();
    }
    // Curl tip — black cylinder then white fluffy tip ball
    glTranslatef(0.0f, 0.0f, 0.50f);
    glRotatef(40.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.13f, 0.04f, 0.28f, 8, 1);
    glTranslatef(0.0f, 0.0f, 0.28f);
    drawFluffyEllipsoid(0.14f, 0.14f, 0.14f, 0.28f, catWhiteColor, 6, 8);
    glPopMatrix();

    // --- Legs (animated trot gait) ---
    glColor3f(0.92f, 0.90f, 0.87f);
    float legX[4]        = { -0.32f,  0.32f, -0.32f,  0.32f };
    float legZ[4]        = { -0.45f, -0.45f,  0.45f,  0.45f };
    float phaseOffset[4] = { 0.0f, (float)M_PI, (float)M_PI, 0.0f };
    const float swingAmp = 18.0f;

    for (int i = 0; i < 4; i++)
    {
        float swing = sinf(cat.legPhase + phaseOffset[i]) * swingAmp;
        glPushMatrix();
        glTranslatef(legX[i], -0.36f, legZ[i]);
        glRotatef(swing, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, 0.10f, 0.08f, 0.45f, 7, 1);
        // Small fluffy paw at the end
        glTranslatef(0.0f, 0.0f, 0.45f);
        drawFluffyEllipsoid(0.12f, 0.09f, 0.10f, 0.14f, catWhiteColor, 6, 8);
        glPopMatrix();
    }

    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawCat()
{
    float x, z, yaw;
    getCatTransform(x, z, yaw);
    float ground = terrainHeight(x, z);
    if (ground < 0.0f) ground = 0.0f;
    drawCatAt(x, ground + 0.81f, z, yaw);
}

// =========================================================================
// Kittens — shared geometry renderer + two colour schemes
// =========================================================================

// --- Tuxedo colours ---
static void tuxBodyColor(float x, float y, float z)
{
    float bib = -z - y * 0.50f - 0.12f + 0.05f * sinf(x * 4.0f);
    if (bib > 0.0f) glColor3f(0.97f, 0.97f, 0.97f);
    else            glColor3f(0.12f, 0.12f, 0.12f);
}
static void tuxHeadColor(float x, float y, float z)
{
    (void)x;
    if (y < (-0.08f - z * 0.18f)) glColor3f(0.97f, 0.97f, 0.97f);
    else                           glColor3f(0.12f, 0.12f, 0.12f);
}
static void tuxLightColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.97f, 0.97f, 0.97f); }
static void tuxDarkColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.12f, 0.12f, 0.12f); }

// --- Orange tabby colours ---
static void orBodyColor(float x, float y, float z)
{
    float bib = -z - y * 0.50f - 0.12f + 0.05f * sinf(x * 4.0f);
    if (bib > 0.0f) {
        glColor3f(0.97f, 0.94f, 0.85f);  // cream belly
    } else {
        float stripe = sinf(z * 6.0f) * 0.5f + 0.5f;
        glColor3f(0.90f - stripe * 0.12f, 0.50f - stripe * 0.10f, 0.06f);
    }
}
static void orHeadColor(float x, float y, float z)
{
    (void)x;
    if (y < (-0.08f - z * 0.18f)) glColor3f(0.97f, 0.94f, 0.85f);
    else                           glColor3f(0.88f, 0.50f, 0.08f);
}
static void orLightColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.97f, 0.94f, 0.85f); }
static void orDarkColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.78f, 0.36f, 0.04f); }

// --- Shared kitten geometry (called after transform + scale are set up) ---
static void renderKittenGeometry(GLUquadric* q,
    ColorFn bodyFn, ColorFn headFn, ColorFn lightFn, ColorFn darkFn,
    float eyeR, float eyeG, float eyeB,
    float legPhaseScale, float legPhaseShift)
{
    // Body
    drawFluffyEllipsoid(0.58f, 0.43f, 0.95f, 0.13f, bodyFn, 18, 22);

    // Chest bib
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, -0.80f);
    drawFluffyEllipsoid(0.30f, 0.26f, 0.16f, 0.16f, lightFn, 8, 12);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, -1.02f);
    drawFluffyEllipsoid(0.42f, 0.40f, 0.40f, 0.09f, headFn, 12, 16);
    glPopMatrix();

    // Cheek puffs
    glPushMatrix(); glTranslatef(-0.28f, 0.14f, -1.08f);
    drawFluffyEllipsoid(0.18f, 0.15f, 0.13f, 0.16f, lightFn, 7, 10); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.28f, 0.14f, -1.08f);
    drawFluffyEllipsoid(0.18f, 0.15f, 0.13f, 0.16f, lightFn, 7, 10); glPopMatrix();

    // Ears
    darkFn(0,0,0);
    glPushMatrix();
    glTranslatef(-0.22f, 0.57f, -0.98f);
    glRotatef(-18.0f,0,0,1); glRotatef(12.0f,1,0,0);
    gluCylinder(q, 0.11f, 0.01f, 0.26f, 6, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.22f, 0.57f, -0.98f);
    glRotatef( 18.0f,0,0,1); glRotatef(12.0f,1,0,0);
    gluCylinder(q, 0.11f, 0.01f, 0.26f, 6, 1);
    glPopMatrix();

    // Eyes
    glColor3f(eyeR, eyeG, eyeB);
    glPushMatrix(); glTranslatef(-0.15f, 0.24f, -1.37f); gluSphere(q, 0.075f, 8, 6); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.15f, 0.24f, -1.37f); gluSphere(q, 0.075f, 8, 6); glPopMatrix();

    // Pupils
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix(); glTranslatef(-0.15f, 0.24f, -1.44f); gluSphere(q, 0.040f, 6, 4); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.15f, 0.24f, -1.44f); gluSphere(q, 0.040f, 6, 4); glPopMatrix();

    // Nose
    glColor3f(0.90f, 0.50f, 0.55f);
    glPushMatrix(); glTranslatef(0.0f, 0.13f, -1.40f); gluSphere(q, 0.045f, 6, 5); glPopMatrix();

    // Whiskers
    glColor3f(0.90f, 0.90f, 0.90f);
    glPushMatrix(); glTranslatef(-0.10f,0.12f,-1.40f); glRotatef( 8.0f,0,1,0); glRotatef( 90,0,1,0); gluCylinder(q,0.012f,0.006f,0.38f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.10f,0.10f,-1.40f); glRotatef(-5.0f,0,1,0); glRotatef( 90,0,1,0); gluCylinder(q,0.012f,0.006f,0.38f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.10f,0.12f,-1.40f); glRotatef(-8.0f,0,1,0); glRotatef(-90,0,1,0); gluCylinder(q,0.012f,0.006f,0.38f,4,1); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.10f,0.10f,-1.40f); glRotatef( 5.0f,0,1,0); glRotatef(-90,0,1,0); gluCylinder(q,0.012f,0.006f,0.38f,4,1); glPopMatrix();

    // Tail (dark body, light fluffy tip)
    darkFn(0,0,0);
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.94f);
    glRotatef(-50.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.20f, 0.12f, 0.45f, 8, 1);
    for (int i = 0; i <= 3; i++)
    {
        float t = (float)i / 3.0f, r = 0.18f - t * 0.05f;
        glPushMatrix(); glTranslatef(0,0,0.45f*t);
        drawFluffyEllipsoid(r, r, r, 0.20f, darkFn, 6, 9);
        glPopMatrix();
    }
    glTranslatef(0,0,0.45f); glRotatef(38.0f,1,0,0);
    gluCylinder(q, 0.11f, 0.04f, 0.24f, 7, 1);
    glTranslatef(0,0,0.24f);
    drawFluffyEllipsoid(0.12f, 0.12f, 0.12f, 0.26f, lightFn, 6, 8);
    glPopMatrix();

    // Legs
    float legX[4]        = { -0.32f,  0.32f, -0.32f,  0.32f };
    float legZ[4]        = { -0.45f, -0.45f,  0.45f,  0.45f };
    float phaseOffset[4] = { 0.0f, (float)M_PI, (float)M_PI, 0.0f };
    for (int i = 0; i < 4; i++)
    {
        float swing = sinf(cat.legPhase * legPhaseScale + phaseOffset[i] + legPhaseShift) * 18.0f;
        darkFn(0,0,0);
        glPushMatrix();
        glTranslatef(legX[i], -0.36f, legZ[i]);
        glRotatef(swing, 1,0,0); glRotatef(90,1,0,0);
        gluCylinder(q, 0.10f, 0.08f, 0.32f, 6, 1);
        glTranslatef(0,0,0.32f);
        gluCylinder(q, 0.08f, 0.07f, 0.13f, 5, 1);
        glTranslatef(0,0,0.13f);
        drawFluffyEllipsoid(0.12f, 0.09f, 0.10f, 0.14f, lightFn, 6, 8);
        glPopMatrix();
    }
}

void drawKittenAt(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(KITTEN_SCALE, KITTEN_SCALE, KITTEN_SCALE);
    glDisable(GL_TEXTURE_2D);
    GLUquadric* q = gluNewQuadric();
    renderKittenGeometry(q, tuxBodyColor, tuxHeadColor, tuxLightColor, tuxDarkColor,
                         0.90f, 0.70f, 0.10f, 1.25f, 0.8f);
    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawKitten()
{
    float angle  = cat.angle - KITTEN_FOLLOW_OFFSET;
    float x      = cosf(angle) * CAT_CIRCLE_RADIUS;
    float z      = sinf(angle) * CAT_CIRCLE_RADIUS;
    float yaw    = atan2f(-sinf(angle), cosf(angle)) * 180.0f / (float)M_PI;
    float ground = terrainHeight(x, z);
    if (ground < 0.0f) ground = 0.0f;
    drawKittenAt(x, ground + 0.81f * KITTEN_SCALE, z, yaw);
}

void drawOrangeKittenAt(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(KITTEN2_SCALE, KITTEN2_SCALE, KITTEN2_SCALE);
    glDisable(GL_TEXTURE_2D);
    GLUquadric* q = gluNewQuadric();
    renderKittenGeometry(q, orBodyColor, orHeadColor, orLightColor, orDarkColor,
                         0.25f, 0.75f, 0.20f, 1.40f, 2.1f);
    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawOrangeKitten()
{
    float angle  = cat.angle - KITTEN_FOLLOW_OFFSET;
    float radius = CAT_CIRCLE_RADIUS + 1.8f;   // one slot outward from tuxedo
    float x      = cosf(angle) * radius;
    float z      = sinf(angle) * radius;
    float yaw    = atan2f(-sinf(angle), cosf(angle)) * 180.0f / (float)M_PI;
    float ground = terrainHeight(x, z);
    if (ground < 0.0f) ground = 0.0f;
    drawOrangeKittenAt(x, ground + 0.81f * KITTEN2_SCALE, z, yaw);
}

// =========================================================================
// White kitten
// =========================================================================
static void whiteBodyColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.97f, 0.96f, 0.94f); }

static void whiteHeadColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.97f, 0.96f, 0.94f); }

static void whiteLightColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.97f, 0.96f, 0.94f); }

static void whiteDarkColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.88f, 0.80f, 0.80f); }  // faint pink for ears/tail

void drawWhiteKittenAt(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(KITTEN3_SCALE, KITTEN3_SCALE, KITTEN3_SCALE);
    glDisable(GL_TEXTURE_2D);
    GLUquadric* q = gluNewQuadric();
    renderKittenGeometry(q, whiteBodyColor, whiteHeadColor, whiteLightColor, whiteDarkColor,
                         0.30f, 0.60f, 0.95f, 1.15f, 1.4f);   // blue eyes
    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawWhiteKitten()
{
    float angle  = cat.angle - KITTEN_FOLLOW_OFFSET;
    float radius = CAT_CIRCLE_RADIUS - 1.8f;   // one slot inward from tuxedo
    float x      = cosf(angle) * radius;
    float z      = sinf(angle) * radius;
    float yaw    = atan2f(-sinf(angle), cosf(angle)) * 180.0f / (float)M_PI;
    float ground = terrainHeight(x, z);
    if (ground < 0.0f) ground = 0.0f;
    drawWhiteKittenAt(x, ground + 0.81f * KITTEN3_SCALE, z, yaw);
}

// =========================================================================
// Gray kitten
// =========================================================================
static void grayBodyColor(float x, float y, float z)
{
    (void)x; (void)z;
    if (y < -0.10f) glColor3f(0.76f, 0.76f, 0.78f);   // lighter belly
    else            glColor3f(0.58f, 0.58f, 0.62f);    // steel gray body
}

static void grayHeadColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.58f, 0.58f, 0.62f); }

static void grayLightColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.76f, 0.76f, 0.78f); }

static void grayDarkColor(float x, float y, float z)
{ (void)x;(void)y;(void)z; glColor3f(0.42f, 0.42f, 0.45f); }

void drawGrayKittenAt(float x, float y, float z, float yaw)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw + 180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(KITTEN4_SCALE, KITTEN4_SCALE, KITTEN4_SCALE);
    glDisable(GL_TEXTURE_2D);
    GLUquadric* q = gluNewQuadric();
    renderKittenGeometry(q, grayBodyColor, grayHeadColor, grayLightColor, grayDarkColor,
                         0.88f, 0.68f, 0.10f, 1.30f, 3.5f);   // amber eyes
    gluDeleteQuadric(q);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawGrayKitten()
{
    float angle  = cat.angle - KITTEN_FOLLOW_OFFSET;
    float radius = CAT_CIRCLE_RADIUS - 3.6f;   // two slots inward from tuxedo
    float x      = cosf(angle) * radius;
    float z      = sinf(angle) * radius;
    float yaw    = atan2f(-sinf(angle), cosf(angle)) * 180.0f / (float)M_PI;
    float ground = terrainHeight(x, z);
    if (ground < 0.0f) ground = 0.0f;
    drawGrayKittenAt(x, ground + 0.81f * KITTEN4_SCALE, z, yaw);
}

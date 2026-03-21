#include "bee.h"
#include "utils.h"
#include "terrain.h"

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

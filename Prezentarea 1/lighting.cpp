#include "lighting.h"
#include "utils.h"
#include "terrain.h"

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

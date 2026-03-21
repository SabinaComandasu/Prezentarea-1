#include "utils.h"
#include "stb_image.h"

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

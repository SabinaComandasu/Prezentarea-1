#include "globals.h"
#include "utils.h"
#include "terrain.h"
#include "lighting.h"
#include "scene.h"
#include "bee.h"
#include "input.h"
#include "renderer.h"

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Blossom Haven");

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

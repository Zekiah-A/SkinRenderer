// Basic OpenGL drawiung test
#ifdef __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif
#include "FreeImage.h"
#include <math.h>

// INSTRUCTIONS:
// Depends on freeglut, can be installed on arch with $ sudo pacman -S freeglut
// Also depends on FreeImage, can be installed on arch with $ sudo pacman -S freeimage
// May also depend on libglvnd, can be installed with $ sudo pacman -S libglvnd
// $ gcc main.c -lGL -lGLU -lglut -lfreeimage && ./a.out

// This project can be built on windows with with msys2/mingw (https://www.mingw-w64.org/)

BYTE* textureData;

// Thanks for the resizing logic, and other help and code from Lighthouse3D!
// https://github.com/lighthouse3d/GLUT-Tutorial/blob/master/source/i-thecodesofarV.cpp
void resize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0) {
		h = 1;
    }
    if (w == 0) {
        w = 1;
    }

	float ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

float i = 0;

// We take head as to be 1 x 1 x 1
// Arm width and depth = 0.5 x 0.5
// Height of arms, legs and torso are all 1.5x height of head
void renderScene() {
    // Clear screen and enable depth test (like shading stuff)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    // Pushing matrix allows us to manipulate the world transforms, we pop after transforming and drawing
    // any object so that the next draw call will not be affected (as otherwise, they would compound)
    glPushMatrix();

    i += 0.01f;

    // Camera
    // angle of rotation for the camera direction
    float angle = 30.0;
    float lookingX = 1.0f + cos(i), lookingZ = -1.0f;
    float positionX = -3.0f, positionZ = -6.5f;
	gluLookAt(positionX, 1.0f, positionZ, positionX + lookingX, 1.0f,  positionZ + lookingZ, 0.0f, 1.0f,  0.0f);

    // Tetxure
    int format = (FI_RGBA_RED == 0) ? GL_RGB : GL_BGR;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, format, GL_UNSIGNED_BYTE, textureData);

    // Head
    glPushMatrix();
        glTranslatef(0, 1.25, -10);
        //glColor3f(1, 0.7, 0.3);
        glutSolidCube(1);
    glPopMatrix();

    // Arm Left
    glPushMatrix();
        glTranslatef(-0.75, 0, -10);
        //glColor3f(0.2, 0.5, 0.3);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1);
    glPopMatrix();

    // Arm Right
    glPushMatrix();
        glTranslatef(0.75, 0, -10);
        //glColor3f(0.2, 0.5, 0.3);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1);
    glPopMatrix();

    // Torso
    glPushMatrix();
        glTranslatef(0, 0, -10);
        //glColor3f(0.2, 0.6, 0.3);
        glScalef(1, 1.5, 0.5);
        glutSolidCube(1);
    glPopMatrix();

    // Leg left
    glPushMatrix();
        glTranslatef(-0.25, -1.5, -10);
        //glColor3f(0.5, 0.7, 0.9);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1);
    glPopMatrix();

    // Leg right
    glPushMatrix();
        glTranslatef(0.25, -1.5, -10);
        //glColor3f(0.3, 0.7, 1);
        glScalef(0.5, 1.5, 0.5);
        glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();
	glutSwapBuffers();
}

void initialise() {
    // Enable things that will make the lighting look good
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glDepthFunc(GL_LEQUAL);    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Create ambient world lights
    GLfloat ambientLight[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Create a omnilight, play around with these values to change character lighting
    GLfloat lightPosition[] = { 0.0, 0.0, 2.0, -0.5, 1.0 }; 
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Load images
    FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, "skin.png", 0);
    FIBITMAP *texture = FreeImage_ConvertTo24Bits(bitmap);
    textureData = FreeImage_GetBits(texture);
    FreeImage_Unload(bitmap);

    // Aply neearest neighbour filtering & tetxure cordinate generation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(1000, 350);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Minecraft skin render");
    initialise();

	// register callbacks
	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene); //whatever fps
    glutReshapeFunc(resize);

	// enter GLUT event processing cycle
	glutMainLoop();
	
	return 0;
}
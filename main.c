#include <GL/glew.h>
#ifdef __APPLE__
    #include <GLUT/freeglut.h>
#else
    #include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <FreeImage.h>
#include <string.h>
#include "types.h"
#include "shader.h"
#include <GL/gl.h>
#include <GL/glu.h>

// INSTRUCTIONS:
// Depends on freeglut, can be installed on arch with $ sudo pacman -S freeglut
// Also depends on FreeImage, can be installed on arch with $ sudo pacman -S freeimage
// Also depends on GLEW, can be installed on arch with $ sudo pacman -S glew
// May also depend on libglvnd, can be installed with $ sudo pacman -S libglvnd
// $ gcc -o skin-renderer main.c types.h types.c -lGLEW -lGL -lGLU -lglut -lfreeimage && ./skin-renderer

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

// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
int loadModel() {
    // These hold data from all other entries in obj file
    vector* tempVerticies = newVector(sizeof(vector3), 50);
    vector* tempUvs = newVector(sizeof(vector2), 50);
    vector* tempNormals = newVector(sizeof(vector3), 50);

    // These are for holding data from f (faces) within the obj file
    vector* vertexIndicies = newVector(sizeof(unsigned int), 50);
    vector* uvIndicies = newVector(sizeof(unsigned int), 50);
    vector* normalIndicies = newVector(sizeof(unsigned int), 50);

    FILE* file = fopen("player.obj", "r");
    if (file == NULL) {
        perror("Could not load file from path.");
        return -1;
    }

    while(1) {
        // Read first word of file, format is always starting with letter, then data
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF){
            break;
        }

        // Vertex
        if (strcmp(lineHeader, "v") == 0 ) {
            vector3* vertex = malloc(sizeof(vector3));
            fscanf(file, "%f %f %f\n", &vertex->x, &vertex->y, &vertex->z);
            addToVector(tempVerticies, (char*) vertex);
        }
        // UV
        else if (strcmp(lineHeader, "vt") == 0) {
            vector2* uv = malloc(sizeof(vector2));
            fscanf(file, "%f %f\n", &uv->x, &uv->y);
            addToVector(tempUvs, (char*) uv);
        }
        // Normal
        else if (strcmp(lineHeader, "vn") == 0) {
            vector3* normal = malloc(sizeof(vector3));
            fscanf(file, "%f %f %f\n", &normal->x, &normal->y, &normal->z );
            addToVector(tempNormals, (char*) normal);
        }
        //Face indexes (use indexes from previous to make actual mesh)
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3];
            unsigned int uvIndex[3];
            unsigned int normalIndex[3];
            
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9){
                perror("File can't be read by object parser. Try exporting with "
                       "other options (may not be exported as triangulated mesh).\n");
                return -1;
            }

            addToVector(vertexIndicies, (unsigned char*) &vertexIndex[0]);
            addToVector(vertexIndicies, (unsigned char*) &vertexIndex[1]);
            addToVector(vertexIndicies, (unsigned char*) &vertexIndex[2]);
            addToVector(uvIndicies, (unsigned char*) &uvIndex[0]);
            addToVector(uvIndicies, (unsigned char*) &uvIndex[1]);
            addToVector(uvIndicies, (unsigned char*) &uvIndex[2]);
            addToVector(normalIndicies, (unsigned char*) &normalIndex[0]);
            addToVector(normalIndicies, (unsigned char*) &normalIndex[1]);
            addToVector(normalIndicies, (unsigned char*) &normalIndex[2]);
        }
    }

    vector3* outputVerticies = malloc(vertexIndicies->elementsCount * sizeof(vector3));
    vector2* outputUvs = malloc(uvIndicies->elementsCount * sizeof(vector2));
    vector3* outputNormals = malloc(normalIndicies->elementsCount * sizeof(vector3));

    // Faces are defined using lists of vertex, texture and normal indices
    // in the format vertex_index/texture_index/normal_index for which each
    // index starts at 1 and increases corresponding to the order in which the
    // referenced element was defined, unpack these to get the final output verticies in order.
    for (unsigned int i = 0; i < vertexIndicies->elementsCount; i++) {
        unsigned int vertexIndex = *(unsigned int*) getFromVector(vertexIndicies, i);
        vector3 vertex = *(vector3*) getFromVector(tempVerticies, vertexIndex - 1);
        outputVerticies[i] = vertex;
    }

    for (unsigned int i = 0; i < uvIndicies->elementsCount; i++) {
        unsigned int uvIndex = *(unsigned int*) getFromVector(uvIndicies, i);
        vector2 uv = *(vector2*) getFromVector(tempUvs, uvIndex - 1);
        outputUvs[i] = uv;
    }

    for (unsigned int i = 0; i < normalIndicies->elementsCount; i++) {
        unsigned int normalIndex = *(unsigned int*) getFromVector(normalIndicies, i);
        vector3 normal = *(vector3*) getFromVector(tempNormals, normalIndex - 1);
        outputNormals[i] = normal;
    }

    // Setup textures
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    int format = (FI_RGBA_RED == 0) ? GL_RGB : GL_BGR;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, format, GL_UNSIGNED_BYTE, textureData);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Setup shaders
    GLenum shaderTypes[] = { GL_FRAGMENT_SHADER, GL_VERTEX_SHADER };
    GLchar* shaderNames[] = { "fragment.glsl", "vertex.glsl" };
	GLuint shaderProgramID = loadShader(shaderTypes, shaderNames, 2);
    glUseProgram(shaderProgramID);

    // Bind the texture ID to the shader using a texture sampler uniform
    glUniform1i(glGetUniformLocation(shaderProgramID, "textureSampler"), 0); // our program only has 1 shader per - dodgy
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Generate a new vertex VBO and get the associated ID
    GLuint vertexBufferId;
    glGenBuffers(1, &vertexBufferId);    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertexIndicies->elementsCount * sizeof(vector3), outputVerticies, GL_STATIC_DRAW);
    
    // Generate new UV VBO and get the associated ID
    GLuint uvBufferId;
    glGenBuffers(1, &uvBufferId);    
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
    glBufferData(GL_ARRAY_BUFFER, uvIndicies->elementsCount * sizeof(vector2), outputUvs, GL_STATIC_DRAW);

    // Vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    // attribute, size, type, normalised, stride, array buffer offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    // Draw everything
    glDrawArrays(GL_TRIANGLES, 0, vertexIndicies->elementsCount);
    return 0;
}

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

    // Camera
    // angle of rotation for the camera direction
    /*
    float angle = 30.0;
    float lookingX = 1.0f, lookingZ = -1.0f;
    float positionX = -3.0f, positionZ = -6.5f;
    */
    float angle = 30.0;
    float lookingX = 0.8f, lookingZ = -1.0f;
    float positionX = -8.0f, positionZ = 10.0f, positionY = -1.4f;
	gluLookAt(positionX, -positionY, positionZ, positionX + lookingX, 1.0f,  positionZ + lookingZ, 0.0f, 1.0f,  0.0f);

    //glColor3f(0, 0.4f, 1);
    glutSolidCube(1.0f);
    loadModel();

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
    FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, "a.png", 0);
    FIBITMAP *texture = FreeImage_ConvertTo24Bits(bitmap);
    textureData = FreeImage_GetBits(texture);
    FreeImage_Unload(bitmap);

    // Aply neearest neighbour filtering & tetxure cordinate generation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glEnable(GL_TEXTURE_GEN_S);
    //glEnable(GL_TEXTURE_GEN_T);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(1000, 350);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Minecraft skin render");
    glewInit();
    initialise();

	// register callbacks
	glutDisplayFunc(renderScene);
    //glutIdleFunc(renderScene);
    glutReshapeFunc(resize);

	// enter GLUT event processing cycle
	glutMainLoop();
	
	return 0;
}
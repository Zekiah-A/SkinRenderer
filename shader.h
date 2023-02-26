#ifdef __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

int loadShader(GLenum* shaderTypes, char** fileNames, int count);
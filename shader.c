#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "types.h"

// https://github.com/opengl-tutorials/ogl/blob/master/common/shader.cpp#L17
int loadShader(GLenum* shaderTypes, GLchar** fileNames, int count) {
    // Create the program
    int result = 0;
    int infoLogLength;
	GLuint programID = glCreateProgram();

    for (int i = 0; i < count; i++) {
        if (shaderTypes[i] != GL_VERTEX_SHADER && shaderTypes[i] != GL_FRAGMENT_SHADER) {
            printf("ST -> %d\n\n", shaderTypes[i]);
            perror("Invalid shader type! Must be of type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER");
            return -1;
        }

        // Read shader file
        FILE* shaderFile = fopen(fileNames[i], "r");
        if (shaderFile == NULL) {
            perror("Could not find shader file at specified path!");
            return -1;
        }

        fseek(shaderFile, 0L, SEEK_END);
        int fileLength = ftell(shaderFile);
        fseek(shaderFile, 0L, SEEK_SET);
        
        GLchar* shaderText = malloc(fileLength);
        fread(shaderText, 1, fileLength, shaderFile);

        // Compile shader
        GLuint shaderID = glCreateShader(shaderTypes[i]);
        glShaderSource(shaderID, 1, (const GLchar* const*) &shaderText, NULL);
        glCompileShader(shaderID);

        // Verify shader validity
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            char* shaderErrorMessages = malloc(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderErrorMessages);
            printf("[shader] %s", shaderText);
            printf("\n%s", shaderErrorMessages);
            return -1;
        }
        
        glAttachShader(programID, shaderID);
    }

	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		char* programErrorMessages = malloc(infoLogLength);
		glGetProgramInfoLog(programID, infoLogLength, NULL, programErrorMessages);
		printf("[program] %s\n", programErrorMessages);
	}

    // We can't really cleanup without saving shaderIDs somewhere
	//glDetachShader(programID, shaderID);	
	//glDeleteShader(shaderID);
	return programID;
}
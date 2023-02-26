#ifdef __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif
#include "types.h"
#include <string.h>

// Vector (dynamic array/list)
vector* newVector(unsigned long elementSize, unsigned long reserveIncrease) {
    vector* vec = malloc(sizeof(vector));
    vec->reserve = 10;
    vec->reserveIncrease = reserveIncrease == 0 ? 10 : reserveIncrease;
    vec->elementSize = elementSize;
    vec->elementsCount = 0;
    vec->elements = (unsigned char*) malloc(vec->reserve * vec->elementSize);
}

void freeVector(vector* vector) {
    free(vector->elements);
    free(vector);
}

void growVector(vector* vector) {
    // Alloc a new elements array with the size + the reserve increase, then
    // copy the old elements to the new elements array, free the old array in the
    // vector and assign it to the new elements & update the reserve

    unsigned char* newElements = (unsigned char*) malloc((vector->reserve + vector->reserveIncrease) * vector->elementSize);
    memcpy(newElements, vector->elements, vector->elementsCount * vector->elementSize);
    free(vector->elements);
    vector->elements = newElements;
    vector->reserve += vector->reserveIncrease;
}

// Supply with a pointer to the vector, and pointer to value
// method will auto resolve value from value pointer
void addToVector(vector* vector, unsigned char* element) {
    if (vector->elementsCount >= vector->reserve) {
        growVector(vector);
    }

    // Add new element to end of vector
    memcpy(vector->elements + (vector->elementsCount * vector->elementSize), element, vector->elementSize);
    vector->elementsCount++;
}

void deleteFromVector(vector* vector, unsigned int index) {
    memcpy(vector->elements + ((index - 1) * vector->elementSize), vector->elements + (index * vector->elementSize), (vector->elementsCount - index - 1) * vector->elementSize);
    vector->elementsCount--;
}

unsigned char* getFromVector(vector* vector, unsigned int index) {
    return vector->elements + (index * vector->elementSize);
}

// Mathematical vector 3
vector3* newVector3(GLfloat x, GLfloat y, GLfloat z) {
    vector3* vec3 = malloc(sizeof(vector3));
    vec3->x = x;
    vec3->y = y;
    vec3->z = z;

    return vec3;
}

// Mathematical vector 2
vector2* newVector2(GLfloat x, GLfloat y) {
    vector2* vec2 = malloc(sizeof(vector2));
    vec2->x = x;
    vec2->y = y;

    return vec2;
}
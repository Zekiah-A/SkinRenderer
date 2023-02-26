#ifdef __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

// Basically a dynamic list, it has elements and a reserve, every
// time the reserve gets full, we reallocate and make it a little bigger.
typedef struct vector {
    // The max element count we can get to before full
    unsigned long reserve;
    // How much we increase reserve by when full, !if this is 0, bad things will happen!
    unsigned long reserveIncrease;
    // The size of each elememt in the array
    unsigned long elementSize;
    // The number of elements in the arary
    unsigned long elementsCount;
    // The actual elements data, has to be casted to whatever
    unsigned char* elements;
} vector;

// Mathematical idea of a vector
typedef struct vector3 {
    float x;
    float y;
    float z;
} vector3;

typedef struct vector2 {
    float x;
    float y;
} vector2;

vector* newVector(unsigned long elementSize, unsigned long reserveIncrease);
void freeVector(vector* vector);
void growVector(vector* vector);
void addToVector(vector* vector, unsigned char* element);
void deleteFromVector(vector* vector, unsigned int index);
unsigned char* getFromVector(vector* vector, unsigned int index);

vector3* newVector3(GLfloat x, GLfloat y, GLfloat z);

vector2* newVector2(GLfloat x, GLfloat y);
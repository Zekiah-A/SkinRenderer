# SkinRenderer
A horrible OpenGL toy project, written primarily in C, with the aim being to render a 3d minecraft skin.

### Building:
Depends on freeglut, FreeImage, GLEW and libglvnd, can be installed on arch with:

`$ sudo pacman -S freeglut freeimage glew libglvnd`

To compile, run the following

`$ gcc -o skin-renderer main.c types.h types.c -lGLEW -lGL -lGLU -lglut -lfreeimage`

And to run the program, enter

`$ ./skin-renderer`

This project can be built on windows with with msys2/mingw (https://www.mingw-w64.org/)
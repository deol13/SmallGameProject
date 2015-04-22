#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include "Vertex.h"
#include <gl/glew.h>

struct MenuButton
{
public:
	GLuint texture;
	Vertex vertexArr[4];
};

#endif
#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include "Vertex.h"
#include <gl/glew.h>

struct MenuButton
{
public:
	int textureIndex;
	Vertex vertexArr[4];
};

#endif
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "lua.hpp"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

#include "MenuButton.h"

class GuiManager
{
	private:
		int state;
		int error;
		lua_State* L;
		MenuButton* guiButtons;
		int nrOfbuttons;

	public:
		GuiManager();
		~GuiManager();
		void mouseClick(int x, int y);
		void render();
		void clean();
		void init(std::string filename);	
		void createVertexBuffer();

		GLuint getTexture(int index);
		
		GLuint* guiAttribute;
		GLuint* guiBuffer;

};

#endif
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "lua.hpp"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

class GuiManager
{
	private:
		int state;
		int error;
		lua_State* L;

	public:
		GuiManager();
		~GuiManager();
		void mouseClick(int x, int y);
		void render();
};

#endif
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "lua.hpp"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

#include "GuiShader.h"
#include "MenuButton.h"
#include <vector>
#include "stb_image.h"

class GuiManager
{
	private:
		bool grayContButton;
		int texCounter;
		int windowWidth;
		int windowHeight;
		int noAction;
		int error;
		lua_State* L;
		int nrOfbuttons;
		std::vector<MenuButton> guiButtons;
		std::vector<GLuint> textures;

		GuiShader* gShader;
		GLuint gGuiShader;

		Vertex tmp[4];

	public:
		GuiManager(int w, int h);
		~GuiManager();
		void startMenuR(); //Render start menu
		void init();
		int mouseClick(float mx, float my);
		void update();
		void getLuaTable(int nrOfParameters);
		void clean();
		void createVertexBuffer();
		void createTexture(std::string fileName);

		void checkContinueButton();

		void pauseGame();
		void won();
		void defeat();

		static int stateChange(lua_State *L);
		static int nonTableAction;

		void loadTextures();
		GLuint getTexture(int index);
		
		GLuint* guiAttribute;
		GLuint* guiBuffer;

		int state;
};

#endif
#ifndef SHOPUI_H
#define SHOPUI_H

#include "lua.hpp"
#include "stb_image.h"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

#include <vector>
#include "MenuButton.h"
#include "GuiShader.h"
#include "Player.h"

class ShopUI
{
	private:
		std::vector<MenuButton> guiObjects;
		std::vector<GLuint> textures;
		int nrOfObjects;
		int texCounter;
		int newTexture;
		int whichObject;
		int newGold;

		GuiShader* gShader;
		GLuint gGuiShader;

		int error;
		lua_State* L;
		int state;
	public:
		ShopUI();
		~ShopUI();

		void init();
		void update();

		static int stateChange(lua_State *L);
		static int continueOn;

		int mouseClick(float mx, float my, int &gold, Player* player);

		void getNewTexture();
		void getLuaTable();
		void createVertexBuffer();
		void createTexture(std::string fileName);
		void loadTextures();

		void setHealingInLua(Player* player);
		void setState();
		int getState();
		lua_State* getL();

		void showGold(int gold);

		void getFileLuaTable(lua_State *L);
		void setSavedGameInfo(int sword, int spear, int health, int armor);

		GLuint* guiAttribute;
		GLuint* guiBuffer;
};

#endif
#ifndef INGAMEGUI_H
#define INGAMEGUI_H

#include "lua.hpp"
#include "stb_image.h"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

#include <vector>
#include "MenuButton.h"
#include "GuiShader.h"
#include "Player.h"
class InGameGui
{
	private:
		std::vector<MenuButton> guiObjects;
		std::vector<GLuint> textures;
		int nrOfObjects;
		int texCounter;

		GuiShader* gShader;
		GLuint gGuiShader;

		int currentHealth;
		int maxHealth;

		int combo;
		int whichWeapon;

		int error;
		lua_State* L;

		bool showVictorTex;

	public:
		InGameGui();
		~InGameGui();

		void init();
		void update();
		void increaseCombo();
		void comboLost();
		void addHealth();
		int dmgTaken(float dmg);
		void heal(Player* player);
		void changeWeapon();

		void getLuaTable();
		void createVertexBuffer();
		void createExtraBuffer();
		void createTexture(std::string fileName);
		void loadTextures();

		void getFileLuaTable(lua_State *L, int nrOfHp);

		void setVictor(bool value);

		GLuint* guiAttribute;
		GLuint* guiBuffer;
};

#endif
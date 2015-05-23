#ifndef	MAPEDITOR_H
#define MAPEDITOR_H

#include "lua.hpp"
#include "stb_image.h"
#include <cstdio>
#include <iostream>
#include <glm\glm.hpp>

#include <vector>
#include <string>
#include "GuiShader.h"
#include "MenuButton.h"
#include "GObject.h"

class MapEditor
{
private:
	int choice; //0 = nothing selected
	std::vector<int> objects;

	std::vector<MenuButton> guiObjects;
	std::vector<MenuButton> regularObjects;
	std::vector<GLuint> textures;
	int nrOfUIObjs;
	int nrOfRObj;
	int texCounter;

	GuiShader* gShader;
	GLuint gUIShader;

	int error;
	lua_State* L;

	GLuint* uiAttribute;
	GLuint* uiBuffer;

	GLuint* rObjectAttribute;
	GLuint* rObjectBuffer;

public:
	MapEditor();
	~MapEditor();

	void clean(bool luaCleans);
	void init();
	void loadStart();
	void update();

	int mouseClick(float x, float y);

	void selectObject(const char key);

	void getUIFromLua(bool regularObjects);
	void placeObject(float x, float y);
	void removeAnObject(float x, float y);

	void createVertexBuffer();
	void createExtraBuffer();
	void createTexture(const std::string fileName);
	void loadTextures();
};

#endif
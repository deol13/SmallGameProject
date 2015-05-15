#include "ShopUI.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

int ShopUI::continueOn = 0;

ShopUI::ShopUI()
{
	guiAttribute = nullptr;
	guiBuffer = nullptr;
	texCounter = 0;
	nrOfObjects = 0;
	state = 0;

	init();
}
ShopUI::~ShopUI()
{
	guiObjects.clear();

	if (guiBuffer != nullptr)
	{
		glDeleteBuffers(nrOfObjects, guiBuffer);
		delete[] guiBuffer;
		guiBuffer = nullptr;
	}
	if (guiAttribute != nullptr)
	{
		glDeleteVertexArrays(nrOfObjects, guiAttribute);
		delete[] guiAttribute;
		guiAttribute = nullptr;
	}

	delete gShader;
	glDeleteShader(gGuiShader);
	if (L != nullptr)
		lua_close(L);
}

void ShopUI::init()
{
	gShader = new GuiShader(&gGuiShader);
	loadTextures();

	error = 0;
	L = luaL_newstate();
	luaL_openlibs(L);
	error = luaL_loadfile(L, "gui.lua") || lua_pcall(L, 0, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	lua_pushcfunction(L, stateChange);
	lua_setglobal(L, "stateChange");

	lua_getglobal(L, "shopInit");

	getLuaTable();
	createVertexBuffer();
}
void ShopUI::update()
{
	glUseProgram(gGuiShader);

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < nrOfObjects; i++)
	{
		glBindVertexArray(guiAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, guiBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, textures[guiObjects[i].textureIndex]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		GLenum error1 = glGetError();
		if (error1 != GL_NO_ERROR)
			printf("Error");
	}
}

int ShopUI::stateChange(lua_State *L)
{
	continueOn = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return 0;
}

int ShopUI::mouseClick(float mx, float my, int &gold, Player* player)
{
	continueOn = 0;
	newTexture = -1;
	whichObject = -1;

	lua_getglobal(L, "shopClickCheck");
	lua_pushnumber(L, mx);
	lua_pushnumber(L, my);
	lua_pushinteger(L, gold);

	getNewTexture();

	if (whichObject == 1)	//Wants to upgrade sword
	{
		player->setWeaponUpgrade(SWORD, player->getWeaponUpgrade(SWORD) + 1);
	}
	else if (whichObject == 2)	//Wants to upgrade spear
	{
		player->setWeaponUpgrade(SPEAR, player->getWeaponUpgrade(SPEAR) + 1);
	}
	else if (whichObject == 3)	//Wants to upgrade health
	{
		player->setMaxHealth(player->getMaxHealth() + 2);
		player->setHealth(player->getMaxHealth());
	}
	else if (whichObject == 4)
	{
		player->setArmour(player->getArmour() + 1);
	}
	else if (whichObject == 5)
	{
		player->setHealth(player->getMaxHealth());
	}

	gold = newGold;

	showGold(gold);

	if (newTexture != -1)
	{
		guiObjects[whichObject].textureIndex = newTexture;
		if (whichObject == 3)
			return 3;
	}
	else if (whichObject == 5)
	{
		guiObjects[whichObject].textureIndex = newTexture;
		return 4;
	}
	else if (continueOn == 2)
	{
		state = 0;
	}
	else if (continueOn == 1)
		return 1;
	return 0;
}

void ShopUI::getNewTexture()
{
	error = lua_pcall(L, 3, 3, 0);
	if (!error)
	{
		newGold = lua_tonumber(L, -1);
		lua_pop(L, 1);
		whichObject = lua_tonumber(L, -1);
		lua_pop(L, 1);
		newTexture = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

void ShopUI::getLuaTable()
{
	int counter = 0;
	int other = 0;
	float x, y, x2, y2, u, v, u2, v2;
	int textureIndex;

	error = lua_pcall(L, 0, LUA_MULTRET, 0);

	if (!error)
	{
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			if (counter == 0)
			{
				x = lua_tonumber(L, -1);
			}
			else if (counter == 1)
			{
				y = lua_tonumber(L, -1);
			}
			else if (counter == 2)
			{
				x2 = lua_tonumber(L, -1);
			}
			else if (counter == 3)
			{
				y2 = lua_tonumber(L, -1);
			}
			else if (counter == 4)
			{
				u = lua_tonumber(L, -1);
			}
			else if (counter == 5)
			{
				v = lua_tonumber(L, -1);
			}
			else if (counter == 6)
			{
				u2 = lua_tonumber(L, -1);
			}
			else if (counter == 7)
			{
				v2 = lua_tonumber(L, -1);
			}
			else if (counter == 8)
			{
				textureIndex = lua_tonumber(L, -1);
				counter = -1;

				Vertex tmp[4] = { { x, y, 1.0f, u, v }, { x, y2, 1.0f, 0.0f, 1.0f }, { x2, y, 1.0f, 1.0f, 0.0f }, { x2, y2, 1.0f, u2, v2 } };
				MenuButton tempButton = { textureIndex, tmp[0], tmp[1], tmp[2], tmp[3] };
				guiObjects.push_back(tempButton);
				nrOfObjects++;
			}
			counter++;

			lua_pop(L, 1);
		}
	}
	else
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}
void ShopUI::createVertexBuffer()
{
	guiAttribute = new GLuint[nrOfObjects];
	guiBuffer = new GLuint[nrOfObjects];

	for (int i = 0; i < nrOfObjects; i++)
	{
		glGenBuffers(1, &guiBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, guiBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &guiObjects[i].vertexArr[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &guiAttribute[i]);
		glBindVertexArray(guiAttribute[i]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}
void ShopUI::createTexture(std::string fileName)
{
	GLuint texture;
	int x, y, n;

	std::string filePath = "Resource/" + fileName;

	unsigned char* textureData = stbi_load(filePath.c_str(), &x, &y, &n, 4);
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	textures.push_back(texture);
	stbi_image_free(textureData);

	texCounter++;

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}
void ShopUI::loadTextures()
{
	createTexture("shopgui/backsground2.png"); //0

	createTexture("shopgui/sword/sword1.png"); //1
	createTexture("shopgui/sword/sword2.png"); //2
	createTexture("shopgui/sword/sword3.png"); //3
	createTexture("shopgui/sword/swordMax.png"); //4
	
	createTexture("shopgui/spear/spear1.png"); //5
	createTexture("shopgui/spear/spear2.png"); //6
	createTexture("shopgui/spear/spear3.png"); //7
	createTexture("shopgui/spear/spearMax.png"); //8
	
	createTexture("shopgui/hp/hp1.png"); //9
	createTexture("shopgui/hp/hp2.png"); //10
	createTexture("shopgui/hp/hp3.png"); //11
	createTexture("shopgui/hp/hpMax.png"); //12
	
	createTexture("shopgui/armor/armor1.png"); //13
	createTexture("shopgui/armor/armor2.png"); //14
	createTexture("shopgui/armor/armorMax.png"); //15
	
	createTexture("shopgui/numbers/0.png"); //16
	createTexture("shopgui/numbers/1.png"); //17
	createTexture("shopgui/numbers/2.png"); //18
	createTexture("shopgui/numbers/3.png"); //19
	createTexture("shopgui/numbers/4.png"); //20
	createTexture("shopgui/numbers/5.png"); //21
	createTexture("shopgui/numbers/6.png"); //22
	createTexture("shopgui/numbers/7.png"); //23
	createTexture("shopgui/numbers/8.png"); //24
	createTexture("shopgui/numbers/9.png"); //25

	createTexture("moregui/continue.png"); //26

	createTexture("shopgui/medicin/medicin.png"); //27
	createTexture("shopgui/medicin/medicingray.png"); //28
}

void ShopUI::setHealingInLua(Player* player)
{
	bool tmp = false;
	if (player->getHealth() == player->getMaxHealth())
	{
		lua_getglobal(L, "setHealingUsed");
		error = lua_pcall(L, 0, 1, 0);
		if (error)
		{
			std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
		else
		{
			tmp = lua_toboolean(L, -1);
			lua_pop(L, 1);
			guiObjects[5].textureIndex = 28;
		}

	}
	else
	{
		guiObjects[5].textureIndex = 27;
	}
}
void ShopUI::setState()
{
	state = 1;
}
int ShopUI::getState()
{
	return state;
}

lua_State* ShopUI::getL()
{
	return L;
}

void ShopUI::showGold( int gold )
{
	int tmp = gold;
	int h = tmp / 100;
	tmp -= h * 100;
	int t = tmp / 10;
	tmp -= t * 10;
	int o = tmp;

	guiObjects[6].textureIndex = h + 16;
	guiObjects[7].textureIndex = t + 16;
	guiObjects[8].textureIndex = o + 16;

}

void ShopUI::setSavedGameInfo(int sword, int spear, int health, int armor)
{
	guiObjects[1].textureIndex = sword + 1;
	guiObjects[2].textureIndex = spear + 5;
	guiObjects[3].textureIndex = health + 9;
	guiObjects[4].textureIndex = armor + 13;
}
#include "InGameGui.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

InGameGui::InGameGui() 
{
	guiAttribute = nullptr;
	guiBuffer = nullptr;
	texCounter = 0;
	nrOfObjects = 0;

	init();
}
InGameGui::~InGameGui() 
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
}

void InGameGui::init() 
{
	currentHealth = 3;
	maxHealth = 3;
	combo = 0;
	whichWeapon = 1;

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

	lua_getglobal(L, "inGameGUI");

	getLuaTable();
	createVertexBuffer();
}

void InGameGui::update() 
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

void InGameGui::increaseCombo() 
{
	if (guiObjects[3].textureIndex < 18)
		guiObjects[3].textureIndex += 1;
}
void InGameGui::comboLost()
{
	guiObjects[3].textureIndex = 8;
}


void InGameGui::addHealth() 
{
	lua_getglobal(L, "addHeart");
	getLuaTable();
	
	if (maxHealth < 6)
	{ //Increase 4 in [] with 1 for every object before the first heart
		maxHealth++;
		currentHealth = maxHealth;

		createExtraBuffer();
		for (int i = 0; i < maxHealth; i++) //Heal the player to max health
			guiObjects[i + 4].textureIndex = 1;
	}
}

int InGameGui::dmgTaken(float dmg)
{ //4,5,6,7,8,9
	//if (currentHealth > 0) //Temporary
	//{ //Increase 3 in [] with 1 for every object before the first heart
	//	guiObjects[currentHealth + 3].textureIndex = 3;
	//	currentHealth--;
	//}

	for (int i = 0; i < dmg; i++)
	{
		if (guiObjects[currentHealth + 3].textureIndex == 1)
		{
			guiObjects[currentHealth + 3].textureIndex = 2;
		}
		else if (guiObjects[currentHealth + 3].textureIndex == 2)
		{
			guiObjects[currentHealth + 3].textureIndex = 3;
			currentHealth--;
		}
	}

	if (currentHealth == 0)
		return 1;
	return 0;
}

void InGameGui::heal( bool fullHeal )
{
	if (!fullHeal)
	{
		if (currentHealth < maxHealth) //Temporary
		{ //Increase 4 in [] with 1 for every object before the first heart
			if (guiObjects[currentHealth + 3].textureIndex == 2)
				guiObjects[currentHealth + 3].textureIndex = 1;
			else
			{
				guiObjects[currentHealth + 4].textureIndex = 1;
				currentHealth++;
			}
			
		}
	}
	else
	{
		for (int i = 0; i < maxHealth; i++) //Heal the player to max health
			guiObjects[i + 4].textureIndex = 1;
		currentHealth = maxHealth;
	}
}

void InGameGui::changeWeapon() 
{
	if (whichWeapon == 1) //Sword selected
	{
		guiObjects[1].textureIndex = 4;
		guiObjects[2].textureIndex = 7;
		whichWeapon = 2;
	}
	else //Spear selected
	{
		guiObjects[1].textureIndex = 5;
		guiObjects[2].textureIndex = 6;
		whichWeapon = 1;
	}
}

void InGameGui::getLuaTable()
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

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void InGameGui::createVertexBuffer()
{
	guiAttribute = new GLuint[nrOfObjects + 3];
	guiBuffer = new GLuint[nrOfObjects + 3];

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

void InGameGui::createExtraBuffer()
{
	glGenBuffers(1, &guiBuffer[nrOfObjects-1]);
	glBindBuffer(GL_ARRAY_BUFFER, guiBuffer[nrOfObjects - 1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &guiObjects[nrOfObjects - 1].vertexArr[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &guiAttribute[nrOfObjects - 1]);
	glBindVertexArray(guiAttribute[nrOfObjects - 1]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void InGameGui::createTexture(std::string fileName) 
{
	GLuint texture;
	int x, y, n;

	std::string filePath = "Resource/InGameGui/" + fileName;

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

void InGameGui::loadTextures()
{
	createTexture("uibckground.png"); //0

	createTexture("wholeheart.png"); //1
	createTexture("halfheart.png");  //2
	createTexture("emptyheart.png"); //3

	createTexture("sword.png"); //4
	createTexture("glowSword.png"); //5
	createTexture("spear.png"); //6
	createTexture("glowSpear.png"); //7

	createTexture("tmp/Combo0.png"); //8
	createTexture("tmp/Combo1.png"); //9
	createTexture("tmp/Combo2.png"); //10
	createTexture("tmp/Combo3.png"); //11
	createTexture("tmp/Combo4.png"); //12
	createTexture("tmp/Combo5.png"); //13
	createTexture("tmp/Combo6.png"); //14
	createTexture("tmp/Combo7.png"); //15
	createTexture("tmp/Combo8.png"); //16
	createTexture("tmp/Combo9.png"); //17
	createTexture("tmp/Combo10.png"); //18

}



#include "GuiManager.h"

int GuiManager::nonTableAction = 0;

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

GuiManager::GuiManager(int w, int h)
{
	grayContButton = false;
	windowWidth = w;
	windowHeight = h;
	nrOfbuttons = 0;
	guiAttribute = nullptr;
	guiBuffer = nullptr;
	noAction = -1;
	gGuiShader = 0;
	texCounter = 0;

	init();
}
GuiManager::~GuiManager()
{
	clean();
	delete gShader;
	glDeleteShader(gGuiShader);
	if (L != nullptr)
		lua_close(L);
}

void GuiManager::clean()
{
	guiButtons.clear();

	if (guiBuffer != nullptr)
	{
		glDeleteBuffers(nrOfbuttons, guiBuffer);
		delete[] guiBuffer;
		guiBuffer = nullptr;
	}
	if (guiAttribute != nullptr)
	{
		glDeleteVertexArrays(nrOfbuttons, guiAttribute);
		delete[] guiAttribute;
		guiAttribute = nullptr;
	}

	nrOfbuttons = 0;
}

void GuiManager::init()
{
	gShader = new GuiShader(&gGuiShader);
	loadTextures();

	error = 0;
	state = 0;
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
}

void GuiManager::startMenuR()
{
	lua_getglobal(L, "init");

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
	
	getLuaTable(0);

	createVertexBuffer();
}

void GuiManager::update()
{
	glUseProgram(gGuiShader);

	if (state != 3)
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	glActiveTexture(GL_TEXTURE0);

	for ( int i = 0; i < nrOfbuttons; i++ )
	{
		glBindVertexArray(guiAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, guiBuffer[i]);
	
		glBindTexture(GL_TEXTURE_2D, textures[guiButtons[i].textureIndex]);
		
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

int GuiManager::mouseClick(float mx, float my)
{
	noAction = -1;
	nonTableAction = -1;

	lua_getglobal(L, "clicked");
	lua_pushnumber(L, mx);
	lua_pushnumber(L, my);
	lua_pushinteger(L, state);

	getLuaTable(3);

	if (nonTableAction != -1)
	{
		state = 0;
		return nonTableAction;
	}
	else
	{
		createVertexBuffer();
	}
	if (state == 1)
	{
		if (grayContButton)
			guiButtons[2].textureIndex = 15;
		else
			guiButtons[2].textureIndex = 2;
	}

	return -1;
}

void GuiManager::checkContinueButton()
{
	lua_getglobal(L, "checkSavedGameFile");
	error = lua_pcall(L, 0, 1, 0);
	if (!error)
	{
		int tmp = lua_tonumber(L, -1);
		lua_pop(L, 1);

		if (tmp == 0) //No save found
		{
			guiButtons[2].textureIndex = 15;
			grayContButton = true;
		}
		else //Save found
		{
			guiButtons[2].textureIndex = 2;
			grayContButton = false;
		}
	}
	else
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

}

void GuiManager::pauseGame()
{
	lua_getglobal(L, "pauseGame");
	
	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
	
	getLuaTable(0);
	
	createVertexBuffer();
}

void GuiManager::won()
{
	lua_getglobal(L, "onVictory");

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");

	getLuaTable(0);

	createVertexBuffer();
}

void GuiManager::defeat()
{
	lua_getglobal(L, "onDeath");

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");

	getLuaTable(0);

	createVertexBuffer();
}

void GuiManager::getLuaTable(int nrOfParameters)
{
	int counter = 0;
	int other = 0;
	float x, y, x2, y2, u, v, u2, v2;
	int textureIndex;

	if (nrOfParameters == 3)
		error = lua_pcall(L, 3, LUA_MULTRET, 0);
	else
		error = lua_pcall(L, 0, LUA_MULTRET, 0);

	if (!error)
	{
		noAction = lua_tonumber(L, -1);
		lua_pop(L, 1);
		if (noAction > 0)
		{
			clean();

			state = noAction;

			lua_pushnil(L);
			while (lua_next(L, -2))
			{
				if(counter == 0)
				{
					x = lua_tonumber(L, -1);
				}
				else if(counter == 1)
				{
					y = lua_tonumber(L, -1);
				}
				else if(counter == 2)
				{
					x2 = lua_tonumber(L, -1);
				}
				else if(counter == 3)
				{
					y2 = lua_tonumber(L, -1);
				}
				else if(counter == 4)
				{
					u = lua_tonumber(L, -1);
				}
				else if(counter == 5)
				{
					v = lua_tonumber(L, -1);
				}
				else if(counter == 6)
				{
					u2 = lua_tonumber(L, -1);
				}
				else if(counter == 7)
				{
					v2 = lua_tonumber(L, -1);
				}
				else if (counter == 8)
				{
					textureIndex = lua_tonumber(L, -1);
					counter = -1;

					Vertex tmp[4] = { { x, y, 1.0f, u, v }, { x, y2, 1.0f, 0.0f, 1.0f }, { x2, y, 1.0f, 1.0f, 0.0f }, { x2, y2, 1.0f, u2, v2 } };
					MenuButton tempButton = { textureIndex, tmp[0], tmp[1], tmp[2], tmp[3] };
					guiButtons.push_back(tempButton);
					nrOfbuttons++;
				}
				counter++;

				lua_pop(L, 1);
			}
		}
		else if (noAction == 0)
		{
			clean();
			state = 0;
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

void GuiManager::createVertexBuffer()
{
	guiAttribute = new GLuint[nrOfbuttons];
	guiBuffer = new GLuint[nrOfbuttons];

	for (int i = 0; i < nrOfbuttons; i++)
	{
		glGenBuffers(1, &guiBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, guiBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*4, &guiButtons[i].vertexArr[0], GL_STATIC_DRAW);
	
		glGenVertexArrays(1, &guiAttribute[i]);
		glBindVertexArray(guiAttribute[i]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float)* 3));
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

GLuint GuiManager::getTexture(int index)
{
	return textures[index];
}

int GuiManager::stateChange(lua_State *L)
{
	nonTableAction = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return 0;
}

void GuiManager::createTexture(std::string fileName)
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

void GuiManager::loadTextures()
{
	createTexture("MenuTextures/backGround.png"); //0
	createTexture("MenuTextures/newgame.png");    //1
	createTexture("MenuTextures/conti.png");	  //2
	createTexture("MenuTextures/howto.png");	  //3
	createTexture("MenuTextures/mapedit.png");    //4
	createTexture("MenuTextures/exit.png");       //5

	createTexture("MenuTextures/howtoplaybk.png");//6
	createTexture("MenuTextures/newback.png");    //7

	createTexture("moregui/continue2.png");        //8
	createTexture("moregui/quit.png");			  //9

	createTexture("moregui/victory.png");         //10
	createTexture("moregui/defeat.png");		  //11
	createTexture("moregui/restartfree.png");     //12
	createTexture("moregui/restart5g.png");       //13
	createTexture("moregui/pause.png");           //14

	createTexture("MenuTextures/contgrey.png"); //15

	
}
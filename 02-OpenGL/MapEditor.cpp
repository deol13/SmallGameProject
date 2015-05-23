#include "MapEditor.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

MapEditor::MapEditor()
{
	choice = 0;
	error = 0;
	uiAttribute = nullptr;
	uiBuffer = nullptr;
	texCounter = 0;
	nrOfUIObjs = 0;
	nrOfRObj = 0;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	init();
}
MapEditor::~MapEditor()
{
	guiObjects.clear();
	textures.clear();
	regularObjects.clear();


	if (uiBuffer != nullptr)
	{
		glDeleteBuffers(nrOfUIObjs, uiBuffer);
		delete[] uiBuffer;
		uiBuffer = nullptr;
	}
	if (uiAttribute != nullptr)
	{
		glDeleteVertexArrays(nrOfUIObjs, uiAttribute);
		delete[] uiAttribute;
		uiAttribute = nullptr;
	}
	
	if (rObjectBuffer != nullptr)
	{
		glDeleteBuffers(nrOfUIObjs, rObjectBuffer);
		delete[] rObjectBuffer;
		rObjectBuffer = nullptr;
	}
	if (rObjectAttribute != nullptr)
	{
		glDeleteVertexArrays(nrOfUIObjs, rObjectAttribute);
		delete[] rObjectAttribute;
		rObjectAttribute = nullptr;
	}

	delete gShader;
	glDeleteShader(gUIShader);
	if (L != nullptr)
		lua_close(L);
}

void MapEditor::clean(bool luaCleans)
{
	regularObjects.clear();
	nrOfRObj = 0;

	if (rObjectBuffer != nullptr)
	{
		glDeleteBuffers(nrOfUIObjs, rObjectBuffer);
		delete[] rObjectBuffer;
		rObjectBuffer = nullptr;
	}
	if (rObjectAttribute != nullptr)
	{
		glDeleteVertexArrays(nrOfUIObjs, rObjectAttribute);
		delete[] rObjectAttribute;
		rObjectAttribute = nullptr;
	}

	rObjectAttribute = new GLuint[30];
	rObjectBuffer = new GLuint[30];

	//Kalla på Lua och rensa där.
	if (luaCleans)
	{
		lua_getglobal(L, "clean");
		error = lua_pcall(L, 0, 0, 0);
	}
}

void MapEditor::init()
{
	gShader = new GuiShader(&gUIShader);
	loadTextures();

	objects.push_back(1);
	objects.push_back(2);
	objects.push_back(3);
	objects.push_back(4);
	objects.push_back(5);
	objects.push_back(6);
	objects.push_back(7);
	objects.push_back(8);
	objects.push_back(9);
	objects.push_back(33);
	objects.push_back(39);
	objects.push_back(21);
	objects.push_back(34);
	objects.push_back(36);
	objects.push_back(41);
	objects.push_back(37);
	objects.push_back(25);
	objects.push_back(31);
	//...

	rObjectAttribute = new GLuint[30];
	rObjectBuffer = new GLuint[30];

	error = 0;
	L = luaL_newstate();
	luaL_openlibs(L);
	error = luaL_loadfile(L, "MapEditorLua.lua") || lua_pcall(L, 0, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void MapEditor::loadStart()
{
	lua_getglobal(L, "mapEditUI");

	error = lua_pcall(L, 0, LUA_MULTRET, 0);

	getUIFromLua(false);
	createVertexBuffer();
}

void MapEditor::update()
{
	glUseProgram(gUIShader);

	glBindVertexArray(uiAttribute[0]);
	glBindBuffer(GL_ARRAY_BUFFER, uiBuffer[0]);

	glBindTexture(GL_TEXTURE_2D, textures[guiObjects[0].textureIndex]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < nrOfRObj; i++)
	{
		glBindVertexArray(rObjectAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, rObjectBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, textures[regularObjects[i].textureIndex]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	for (int i = 1; i < nrOfUIObjs; i++)
	{
		glBindVertexArray(uiAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uiBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, textures[guiObjects[i].textureIndex]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

int MapEditor::mouseClick(float x, float y)
{
	lua_getglobal(L, "mouseClick");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	error = lua_pcall(L, 2, LUA_MULTRET, 0);
	int event = lua_tointeger(L, -1);
	lua_pop(L, 1);

	if (!error)
	{
		if (event == 1) //(Place
			placeObject(x, y);
		else if (event == 3) //Load
		{
			clean(false);
			getUIFromLua(true);
		}
	}
	return event; //0 = save and 2 = back
}

void MapEditor::selectObject(const char key) //keyboard selection
{
	bool found = false;
	int intKey = key;// atoi(&key);
	intKey -= 48;

	if (intKey != choice) //Select
	{
		for (int i = 0; i < objects.size() && !found; i++)
		{
			if (intKey == objects[i])
			{
				choice = intKey;
				found = true;
			}
		}
	}
	else //Deselect
		choice = 0;

}

void MapEditor::getUIFromLua(bool addRObjects)
{
	int counter = 0;
	int other = 0;
	float x, y, x2, y2, u, v, u2, v2;
	int textureIndex;

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

				if (addRObjects)
				{
					Vertex tmp[4] = { { x, y, 1.0f, u, v }, { x, y2, 1.0f, 0.0f, 0.0f }, { x2, y, 1.0f, 1.0f, 1.0f }, { x2, y2, 1.0f, u2, v2 } };
					MenuButton tempButton = { textureIndex, tmp[0], tmp[1], tmp[2], tmp[3] };

					regularObjects.push_back(tempButton);
					createExtraBuffer();
					nrOfRObj++;
				}
				else
				{
					Vertex tmp[4] = { { x, y, 1.0f, u, v }, { x, y2, 1.0f, 0.0f, 1.0f }, { x2, y, 1.0f, 1.0f, 0.0f }, { x2, y2, 1.0f, u2, v2 } };
					MenuButton tempButton = { textureIndex, tmp[0], tmp[1], tmp[2], tmp[3] };

					guiObjects.push_back(tempButton);
					nrOfUIObjs++;
				}
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
}

void MapEditor::placeObject(float x, float y)
{
	bool success = false;
	int number = 0;

	if (choice > 0)
	{
		lua_getglobal(L, "placeObject");
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		lua_pushnumber(L, choice);
		error = lua_pcall(L, 3, LUA_MULTRET, 0);
		if (!error)
		{
			success = lua_toboolean(L, -1);
			lua_pop(L, 1);
			if (success)
				getUIFromLua(true);
		}
		else
		{
			std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
	}
}

void MapEditor::removeAnObject(float x, float y)
{
	lua_getglobal(L, "removeObjectsHandler");
	error = lua_pcall(L, 0, 1, 0);

	if (!error)
	{
		bool worked = lua_toboolean(L, -1);
		lua_pop(L, 1);

		if (worked)
		{
			nrOfRObj--;
			regularObjects.pop_back();
			rObjectBuffer[nrOfRObj] = 0;
			rObjectAttribute[nrOfRObj] = 0;
		}
	}
	else
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

void MapEditor::createVertexBuffer()
{
	uiAttribute = new GLuint[nrOfUIObjs + 1];
	uiBuffer = new GLuint[nrOfUIObjs + 1];

	for (int i = 0; i < nrOfUIObjs; i++)
	{
		glGenBuffers(1, &uiBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uiBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &guiObjects[i].vertexArr[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &uiAttribute[i]);
		glBindVertexArray(uiAttribute[i]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
	}

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void MapEditor::createExtraBuffer()
{
	glGenBuffers(1, &rObjectBuffer[nrOfRObj]);
	glBindBuffer(GL_ARRAY_BUFFER, rObjectBuffer[nrOfRObj]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &regularObjects[nrOfRObj].vertexArr[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &rObjectAttribute[nrOfRObj]);
	glBindVertexArray(rObjectAttribute[nrOfRObj]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void MapEditor::createTexture(const std::string fileName)
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

void MapEditor::loadTextures()
{
	createTexture("MapEditor/defualtGround.png"); //0

	createTexture("MapEditor/1.png"); //1
	createTexture("MapEditor/2.png"); //2
	createTexture("MapEditor/3.png"); //3
	createTexture("MapEditor/4.png"); //4
	createTexture("MapEditor/5.png"); //5
	createTexture("MapEditor/6.png"); //6
	createTexture("MapEditor/7.png"); //7
	createTexture("MapEditor/8.png"); //8
	createTexture("MapEditor/9.png"); //9
	createTexture("MapEditor/Q.png"); //10(Q)
	createTexture("MapEditor/W.png"); //11(W)
	createTexture("MapEditor/E.png"); //12(E)
	createTexture("MapEditor/R.png"); //13(R)
	createTexture("MapEditor/T.png"); //14(T)
	createTexture("MapEditor/Y.png"); //15(Y)
	createTexture("MapEditor/U.png"); //16(U)
	createTexture("MapEditor/I.png"); //17(I)
	createTexture("MapEditor/O.png"); //18(O)
	createTexture("MapEditor/Load.png"); //19 Load
	createTexture("MapEditor/Save.png"); //20 Save
	//createTexture("MapEditor/Back.png"); //20 SAve

	createTexture("MapEditor/Eld.png"); //21 Fire
	createTexture("MapEditor/Hus.png"); //22 House
	createTexture("MapEditor/Wall1Horiz.png"); //23 Wall 1 horiz
	createTexture("MapEditor/Wall1Verti.png"); //24 wall 1 verti
	createTexture("MapEditor/TrappaFront.png"); //25 Stair front
	createTexture("MapEditor/TrappaBack.png"); //26 Stair back
	createTexture("MapEditor/TrappaLeft.png"); //27 Stair left
	createTexture("MapEditor/TrappaRight.png"); //28 Stair right
	createTexture("MapEditor/Bana2Corner.png"); //29 Corner 1
	createTexture("MapEditor/Wall2Up.png"); //30 Wall 2 up
	createTexture("MapEditor/Wall2Right.png"); //31 Wall 2 right
	createTexture("MapEditor/Wall2Left.png"); //32 Wall 2 left
	createTexture("MapEditor/Wall2Down.png"); //33 Wall 2 down
	createTexture("MapEditor/Bana3Corner.png"); //34 Corner 2
	createTexture("MapEditor/Wall3Up.png"); //35 Wall 3 up
	createTexture("MapEditor/Wall3Right.png"); //36 Wall 3 right
	createTexture("MapEditor/Wall3Left.png"); //37 Wall 3 left
	createTexture("MapEditor/Wall3Down.png"); //38 Wall 3 down

	createTexture("MapEditor/Back.png"); //39 Back
	//createTexture("MapEditor/Play.png"); //40 Play
}
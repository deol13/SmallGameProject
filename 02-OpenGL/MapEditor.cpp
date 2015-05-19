#include "MapEditor.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

MapEditor::MapEditor()
{
	choice = 0;
	error = 0;
	uiAttribute = nullptr;
	uiBuffer = nullptr;
	texCounter = 0;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	init();
}
MapEditor::~MapEditor()
{
	guiObjects.clear();
	textures.clear();

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

	delete gShader;
	glDeleteShader(gUIShader);
	if (L != nullptr)
		lua_close(L);
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
	objects.push_back(65);
	objects.push_back(71);
	objects.push_back(53);
	objects.push_back(66);
	objects.push_back(68);
	objects.push_back(73);
	objects.push_back(69);
	objects.push_back(57);
	objects.push_back(63);
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

	lua_getglobal(L, "mapEditUI");

	error = lua_pcall(L, 0, LUA_MULTRET, 0);

	getUIFromLua(false);
	createVertexBuffer();

	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void MapEditor::update()
{
	glUseProgram(gUIShader);

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < nrOfUIObjs; i++)
	{
		glBindVertexArray(uiAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uiBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, textures[guiObjects[i].textureIndex]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	for (int i = 0; i < nrOfRObj; i++)
	{
		glBindVertexArray(rObjectAttribute[i]);
		glBindBuffer(GL_ARRAY_BUFFER, rObjectBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, textures[regularObjects[i].textureIndex]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	GLenum error1 = glGetError();
	if (error1 != GL_NO_ERROR)
		printf("Error");
}

void MapEditor::selectObject(const char key) //keyboard selection
{
	bool found = false;
	int intKey = atoi(&key);
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

void MapEditor::load() 
{
	lua_getglobal(L, "loadMap");
	error = lua_pcall(L, 0, LUA_MULTRET, 0);
	
	getUIFromLua(true);
}

void MapEditor::save()
{
	lua_getglobal(L, "saveMap");
	error = lua_pcall(L, 0, 0, 0);

	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
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

				Vertex tmp[4] = { { x, y, 1.0f, u, v }, { x, y2, 1.0f, 0.0f, 1.0f }, { x2, y, 1.0f, 1.0f, 0.0f }, { x2, y2, 1.0f, u2, v2 } };
				MenuButton tempButton = { textureIndex, tmp[0], tmp[1], tmp[2], tmp[3] };
				if (addRObjects)
				{
					regularObjects.push_back(tempButton);
					createExtraBuffer();
					nrOfRObj++;
				}
				else
				{
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

void MapEditor::placeObject(float x, float z)
{
	bool success = false;

	if (choice > 0)
	{
		lua_getglobal(L, "placeObject");
		lua_pushnumber(L, x);
		lua_pushnumber(L, z);
		lua_pushnumber(L, choice);
		error = lua_pcall(L, 2, LUA_MULTRET, 0);

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

//Not Done
void MapEditor::removeAnObject()
{
	if (choice > 0)
	{

		lua_getglobal(L, "removeObject");
		error = lua_pcall(L, 0, 0, 0);

		if (!error)
		{
			nrOfRObj--;
		}
		else
		{
			std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
	}
}

void MapEditor::createVertexBuffer()
{
	uiAttribute = new GLuint[nrOfUIObjs];
	uiBuffer = new GLuint[nrOfUIObjs];

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

void MapEditor::loadTextures()
{
	createTexture("andrabanansand.png"); //0

	createTexture("1.png"); //1
	createTexture("2.png"); //2
	createTexture("3.png"); //3
	createTexture("4.png"); //4
	createTexture("5.png"); //5
	createTexture("6.png"); //6
	createTexture("7.png"); //7
	createTexture("8.png"); //8
	createTexture("9.png"); //9
	createTexture("Q.png"); //10(Q)
	createTexture("W.png"); //11(W)
	createTexture("E.png"); //12(E)
	createTexture("R.png"); //13(R)
	createTexture("T.png"); //14(T)
	createTexture("Y.png"); //15(Y)
	createTexture("U.png"); //16(U)
	createTexture("I.png"); //17(I)
	createTexture("O.png"); //18(O)
	createTexture("Load.png"); //19 Load
	createTexture("Save.png"); //20 SAve

	//createTexture("TestAnimation/testtexture.png");
	//createTexture("blocks.png");
	//createTexture("enemyobj/wolflowpoly.png");
	//createTexture("blacktest.png");
	//createTexture("hus/husimitten.png");
	//createTexture("eldpelare/eld.png");
}
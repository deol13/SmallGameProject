#include "GuiManager.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

GuiManager::GuiManager()
{
}
GuiManager::~GuiManager()
{
	clean();
}

void GuiManager::clean()
{
	if (guiButtons != nullptr)
	{
		delete[] guiButtons;
		guiButtons = nullptr;
	}
	if (guiAttribute != nullptr)
	{
		delete[] guiAttribute;
		guiAttribute = nullptr;
	}
	if (guiBuffer != nullptr)
	{
		delete[] guiBuffer;
		guiBuffer = nullptr;
	}
}

void GuiManager::init(std::string filename)
{
	error = 0;
	state = 1;
	L = luaL_newstate();
	luaL_openlibs(L);
	error = luaL_loadfile(L, "LuaButton.lua") || lua_pcall(L, 0, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

void GuiManager::mouseClick(int x, int y)
{
	lua_getglobal(L, "luafunktion");
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);

	error = lua_pcall(L, 0, 0, 0);

	//retur hur många knappar
	//Data för knapparna

	clean();
	createVertexBuffer();
}

void GuiManager::createVertexBuffer()
{
	guiButtons = new MenuButton[nrOfbuttons];
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float)* 3));
	}
}

GLuint GuiManager::getTexture(int index)
{
	return guiButtons[index].texture;
}
#include "GuiManager.h"


GuiManager::GuiManager()
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
GuiManager::~GuiManager()
{
	delete L;
}

void GuiManager::mouseClick(int x, int y)
{
	lua_getglobal(L, "luafunktion");
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
}
void GuiManager::render()
{

}
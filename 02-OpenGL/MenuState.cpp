#include "MenuState.h"

MenuState::MenuState()
{
	menuGui = new GuiManager();
}

MenuState::~MenuState()
{
	delete menuGui;
}


void MenuState::mouseListener(int x, int y)
{
	menuGui->mouseClick(x, y);
}

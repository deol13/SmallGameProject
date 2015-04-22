#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "GuiManager.h"
#include "Render.h"

class MenuState
{
private:
	GuiManager* menuGui;
public:
	MenuState();
	~MenuState();

	void mouseListener(int x, int y);
};

#endif
#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Render.h"
#include "EnemyHandler.h"
#include "Player.h"

class GameState {
private:
	
	Render* render;
	GuiManager* gameUI;
	std::vector<GObject*> renderObjects;
	Player* player;
	std::vector<Enemy> enemyWave;

	
	void loadArena(std::string fileName);
	void spawnEnemies(std::string fileName);
	void spawnPlayer();

public:
	GameState();
	void init();
	void clean();
	void update();
	void keyDown(char c);
	void keyUp(char c);
};
#endif
#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Render.h"
#include <string>
#include "Player.h"

class GameState {
	const int GASIZE = 256;				//Game Area max size
private:
	
	Render* render;
	GuiManager* gameUI;
	std::vector<GObject*> renderObjects;
	std::vector<BoundingRect> collisionVec;
	Player* player;
	//std::vector<Enemy> enemyWave;
	Enemy** enemyWave;
	int waveSize;
	int nrOfArenaObjects;
	int firstEnemyIndex;		//Start of enemies in renderObjects. Useful for enemy deaths etc.

	
	void loadArena(std::string fileName);
	void spawnEnemies(std::string fileName);
	void spawnPlayer();
	bool playerCanMove( Player::Direction dir );

public:
	GameState();
	void init();
	void clean();
	void update();
	void keyDown(char c);
	void keyUp(char c);
	void leftMouseClick(long x, long y);
};
#endif
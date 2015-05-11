#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Render.h"
#include <string>
#include "Player.h"

class GameState {
	const int GASIZE = 256;				//Game Area max size
private:
	int state;
	int** board;
	Render* render;
	GuiManager* gameUI;
	std::vector<GObject*> renderObjects;
	std::vector<BoundingRect> collisionVec;
	Player* player;
	//std::vector<Enemy> enemyWave;
	Enemy** enemyWave;
	int waveSize;
	int enemiesRemaining;
	int nrOfArenaObjects;
	int firstEnemyIndex;		//Start of enemies in renderObjects. Useful for enemy deaths etc.

	void loadArena(std::string fileName);
	void spawnEnemies(std::string fileName);
	void spawnPlayer();
	bool playerCanMove( Player::Direction dir );
	void createNegativePotential(int posX, int posZ, int size);

public:
	GameState(int w, int h);
	~GameState();
	void init(int w, int h);
	void clean();
	void update();
	void keyDown(char c);
	void keyUp(char c);
	void leftMouseClick(long x, long y);
	int getState() const;
};
#endif
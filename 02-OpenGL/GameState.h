#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Render.h"
#include <string>
#include "Player.h"
#include "InGameGui.h"
#include "ShopUI.h"
#include "Enemy.h"

class GameState {
	const int GASIZE = 256;				//Game Area max size
private:
	int state;
	int arenaMap[256][256];	//shows areas covered by buildings
	int waveNumber;
	int gold;
	Render* render;
	GuiManager* menuUI;
	InGameGui* gameUI;
	ShopUI* shopUI;
	std::vector<GObject*> renderObjects;
//	std::vector<BoundingRect> collisionVec;
	Player* player;
	//std::vector<Enemy> enemyWave;
	Enemy** enemyWave;
	int waveSize;
	int enemiesRemaining;
	int nrOfArenaObjects;
	int firstEnemyIndex;		//Start of enemies in renderObjects. Useful for enemy deaths etc.

	bool onExitCleanUp;

	void loadArena(std::string fileName);
	void spawnEnemies(int waveNumber);
	void spawnPlayer();
	bool playerCanMove( Player::Direction dir );
	void nextWave();

	bool realTemp;	//Just for debugging purposes

	static int savedGameInfo(lua_State *L);

public:
	GameState(int w, int h);
	~GameState();
	void init(int w, int h);
	void clean();
	void update();
	void uiUpdate();
	void keyDown(char c);
	void keyUp(char c);
	void leftMouseClick(long x, long y);
	int getState() const;
	int guiState();
	int getShopState();
	int screenClickesOn(float mx, float my);
	void maxHeal();

	void loadSavedGame();
	void saveGame();
};
#endif
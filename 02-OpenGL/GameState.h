#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <windows.h>
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
	float arenaMap[455][256];	//shows areas covered by buildings
	int waveNumber;
	int gold;
	int timer;
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
	int weaponRender;			//index in renderObjects for the player weapon

	bool onExitCleanUp;

	void loadArena(int fileName);
	void spawnEnemies(int waveNumber);
	void spawnPlayer();
	bool playerCanMove( int x, int z );
	void nextWave();

	int* currentMap;
	int* whichSavedWave;
	bool dead;

	static int savedGameInfo(lua_State *L);

public:
	GameState(int w, int h);
	~GameState();
	void init(int w, int h, bool mapEditor);
	void continueInit(int w, int h);
	void clean();
	void cleanedOnDefeat();
	void freeLoad();
	void costLoad();
	void arenaCleanUp();
	void startAfterDefeat();
	void update();
	void uiUpdate();
	void keyDown(char c);
	void keyUp(char c);
	void leftMouseClick(float x, float y);
	void playerAttack();
	int getState() const;
	int guiState();
	int getShopState();
	int screenClickesOn(float mx, float my);
	void maxHeal();

	void loadSavedGame();
	void loadDefeatSavedGame();
	void saveGame();
	void saveGameOnDefeat();

	void setMapEditorMap();
};
#endif
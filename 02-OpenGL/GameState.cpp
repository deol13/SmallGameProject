#include "GameState.h"

GameState::GameState( int w, int h)
{
	currentMap = new int(1);
	whichSavedWave = new int(1);
	gold = 0;
	onExitCleanUp = false;
	//init(w, h);
	//Set render
	render = new Render(GASIZE, w / h);
	render->init(GASIZE, w, h);
	dead = false;
}

GameState::~GameState()
{
	delete render;
	render = nullptr;
	clean();
}

void GameState::init(int w, int h) 
{
	state = 0;
	timer = 0;
	waveNumber = 1;
	//initialize the board the AI uses
	for(int i = 0; i < 256; i++)
	{
		arenaMap[i][0] = -10;
		arenaMap[i][255] = -10;
		arenaMap[0][i] = -10;
		arenaMap[255][i] = -10;
	}
	for (int i = 1; i < 255; i++)
	{
		for(int j = 1; j < 255; j++)
		{
			arenaMap[i][j] = 10;
		}
	}

	enemyWave = nullptr;
	//Load Menu UI
	menuUI = new GuiManager(w, h);
	//Load GUI
	gameUI = new InGameGui();
	//Load Shop UI
	shopUI = new ShopUI();
	//Set player
	spawnPlayer();
	//Load arena
	loadArena(1);
	//Spawn first enemy wave
	spawnEnemies(waveNumber);
	enemiesRemaining = waveSize;

	onExitCleanUp = true;
}

void GameState::continueInit(int w, int h)
{
	state = 0;
	waveNumber = 1;
	//initialize the board the AI uses
	for(int i = 0; i < 256; i++)
	{
		arenaMap[i][0] = 10;
		arenaMap[i][255] = 10;
		arenaMap[0][i] = 10;
		arenaMap[255][i] = 10;
	}
	for(int i = 1; i < 255; i++)
	{
		for(int j = 1; j < 255; j++)
		{
			arenaMap[i][j] = 10;
		}
	}

	enemyWave = nullptr;
	//Load Menu UI
	menuUI = new GuiManager(w, h);
	//Load GUI
	gameUI = new InGameGui();
	//Load Shop UI
	shopUI = new ShopUI();
	//Set player
	spawnPlayer();

	loadSavedGame(); //

	//Load arena
	if (*currentMap == 1)
	{
		loadArena(*currentMap);
		waveNumber = 1;
	}
	else if (*currentMap == 2)
	{
		loadArena(*currentMap);
		waveNumber = 7;
	}
	else if (*currentMap == 3)
	{
		loadArena(*currentMap);
		waveNumber = 13;
	}
	//Spawn first enemy wave
	spawnEnemies(waveNumber);
	enemiesRemaining = waveSize;

	onExitCleanUp = true;
}

void GameState::clean()
{
	if (onExitCleanUp)
	{
		delete gameUI;
		gameUI = nullptr;
		delete shopUI;
		shopUI = nullptr;
		delete menuUI;
		menuUI = nullptr;
		for (int i = 0; i < waveSize; i++)
		{
			delete enemyWave[i];
		}
		delete[] enemyWave;
		enemyWave = nullptr;
		delete player;
		renderObjects.clear();
		onExitCleanUp = false;
		delete currentMap;
		delete whichSavedWave;

		dead = false;
	}
}

void GameState::cleanedOnDefeat()
{
	delete gameUI;
	gameUI = nullptr;
	delete shopUI;
	shopUI = nullptr;
	menuUI->state = 0;
	for (int i = 0; i < waveSize; i++)
	{
		delete enemyWave[i];
		renderObjects.pop_back();
	}
	delete[] enemyWave;
	enemyWave = nullptr;
	waveNumber = 1;
}

void GameState::freeLoad()
{
	dead = false;
	shopUI = new ShopUI();
	gameUI = new InGameGui();

	loadSavedGame();

	if (*currentMap == 1)
		waveNumber = 1;
	else if (*currentMap == 2)
		waveNumber = 7;
	else if (*currentMap == 3)
		waveNumber = 13;


	spawnEnemies(waveNumber);
	enemiesRemaining = waveSize;
	player->setDefault();
}

void GameState::costLoad()
{
	dead = false;
	shopUI = new ShopUI();
	gameUI = new InGameGui();
	gold = -5;

	loadDefeatSavedGame();

	spawnEnemies(*whichSavedWave);
	enemiesRemaining = waveSize;
	player->setDefault();
}

void GameState::arenaCleanUp()
{
	delete enemyWave;
	enemyWave = nullptr;

	for (int i = 0; i < nrOfArenaObjects; i++)	//Cleaning up the arena
	{
		renderObjects.pop_back();
	}
}

void GameState::update()
{
 	player->update();
	
	if (timer == 80)
	{
		nextWave();
		timer = 0;
	}
	if (player->getHealth() <= 0 && !dead )		//Are we dead?
	{
   		dead = true;
 		saveGameOnDefeat();
		menuUI->defeat();
	}
	if (enemiesRemaining <= 0)
	{
 		if (waveNumber == 18) //If we finished the game and / or map
		{
			player->stop(true, true);
			menuUI->won();
		}
		else if( timer == 0)				//Spawn next wave
		{
			timer = 1;
		}
	}
	if (timer != 0)		//All enemies dead so start the timer
	{
		timer++;
	}
	else
	{
		if (menuUI->state != 3 && menuUI->state != 4 && menuUI->state != 5 && shopUI->getState() != 1) //
		{
			for (int i = 0; i < waveSize; i++)
			{
				if (enemyWave[i]->getHealth() >= 0)
				{
					enemyWave[i]->clearPotential(arenaMap);
					enemyWave[i]->setPotential(player->getX(), player->getZ(), 50);
					for (int j = 0; j < waveSize; j++)
					{
						if (i != j)
						{
							enemyWave[i]->setPotential(enemyWave[j]->getX(), enemyWave[j]->getZ(), -5);
						}
					}

					float playerDist = std::sqrt(pow((enemyWave[i]->getX() - player->getX()), 2) +
						pow((enemyWave[i]->getZ() - player->getZ()), 2));

					if (enemyWave[i]->getRange() > playerDist)
					{
						int damage = enemyWave[i]->attack();

						if (player->getInvulTimer() == 0)
						{
							gameUI->dmgTaken(player->takeDamage(damage)); //Deals instant damage to the player and updates the GUI
						}
					}
					else
					{
						enemyWave[i]->move();
					}
					//enemyWave[i]->act(player->getX(), player->getZ(), board);
				}
			}
		}
	}

	render->GeometryPassInit();
	render->render(renderObjects);
	render->lightPass();
	
}

void GameState::uiUpdate()
{
	if (menuUI->state == 3)
	{
		gameUI->update();
		menuUI->update();
	}
	else if (menuUI->state == 4 || menuUI->state == 5)
		menuUI->update();
	else if (shopUI->getState() == 1)
		shopUI->update();
	else
		gameUI->update();
}

void GameState::keyDown(char c)
{
	int moveX = 0;
	int moveZ = 0;
	int tmp = 0;
	bool skipSetDir = false;

	switch (c)
	{
	case 'w':
	case 'W':
		moveZ = 1;
		break; 
	case 's':
	case 'S':
		moveZ = -1;
		break;
	case 'a':
	case 'A':
		moveX = 1;
		break;
	case 'd':
	case 'D':
		moveX = -1;
		break;
	case 'p':
	case 'P':
		player->stop(true, true);
		menuUI->pauseGame();
		break;
	case 'q':
	case 'Q':
		skipSetDir = true;
		gameUI->changeWeapon();

		if (player->getWeapon() == SWORD)
		{
			player->setWeapon(SPEAR);
			renderObjects[weaponRender] = player->getGObject(SPEAR);
		}
		else		//Using spear so change to sword
		{
			player->setWeapon(SWORD);
			renderObjects[weaponRender] = player->getGObject(SWORD);
		}
		break;
	case 'e': //Temporary
	case 'E': //Temporary
		skipSetDir = true; //Temporary
		gameUI->addHealth(); //Temporary
		break; //Temporary
	case 'f': //Temporary
	case 'F': //Temporary
		skipSetDir = true; //Temporary
		maxHeal(); //Temporary
		break; //Temporary
	//case 'z': //Temporary
	//case 'Z': //Temporary
	//	skipSetDir = true; //Temporary
	//	gameUI->increaseCombo(); //Temporary
	//	break; //Temporary
	case 'x': //Temporary
	case 'X': //Temporary
		skipSetDir = true; //Temporary
		gameUI->comboLost(); //Temporary
		break; //Temporary
	case 'm': //Temporary
	case 'M': //Temporary
		skipSetDir = true; //Temporary
		saveGame(); //Temporary
		break; //Temporary
	case 'n': //Temporary
	case 'N': //Temporary
		skipSetDir = true; //Temporary
		loadSavedGame(); //Temporary
		break; //Temporary
	default: 
		break;
	}

	if (!skipSetDir)
	{
		if (playerCanMove(moveX, moveZ))
		{
			player->setMovement(moveX, moveZ);
		}
	}
}
void GameState::keyUp(char c)
{
	if ( c == 'w' || c == 'W' ) {
		player->stop(false, true);
	}
	if ( c == 's' || c == 'S' ) {
		player->stop(false, true);
	}
	if ( c == 'a' || c == 'A' ) {
		player->stop(true, false);
	}
	if ( c == 'd' || c == 'D' ) {
		player->stop(true, false);
	}
}

void GameState::leftMouseClick(float x, float z)
{
	//glm::vec2 vecFromPlayer = glm::vec2(x - player->getX(), y - player->getZ());
	//float angle = atan(vecFromPlayer.y / vecFromPlayer.x);
	//glm::vec2 dirVec =  glm::normalize(glm::vec2(x - player->getX(), y - player->getZ()));
	BoundingPolygon hitbox;

	if (player->getWeapon() == SWORD)
	{
		float weaponRange = 5.0f;	//multiplier for the direction vector
		glm::vec2 dirVec = glm::normalize(glm::vec2(x - player->getX(), z - player->getZ()));

		dirVec.x *= weaponRange;
		dirVec.y *= weaponRange;

		Point points[3];
		points[0] = { player->getX(), player->getZ() };
		points[1] = { player->getX() + dirVec.x - dirVec.y, player->getZ() + dirVec.y + dirVec.x };
		points[2] = { player->getX() + dirVec.x + dirVec.y, player->getZ() + dirVec.y - dirVec.x };
		hitbox = BoundingPolygon(points, 3);
	}
	else		//Using spear
	{
		float weaponRange = 9.0f;
		glm::vec2 dirVec = glm::normalize(glm::vec2(x - player->getX(), z - player->getZ()));

		dirVec.x *= weaponRange;
		dirVec.y *= weaponRange;

		Point point[2];

		point[0] = { player->getX(), player->getZ() };
		point[1] = { player->getX() + dirVec.x, player->getZ() + dirVec.y };
		hitbox = BoundingPolygon(point, 2);
	}

	for(int i = 0; i < waveSize; i++)
	{
		BoundingPolygon test = enemyWave[i]->getBounds();
		bool hit = test.collides(hitbox);
		if(hit)
		{
			int damage = player->getDamageDealt();
			
			if(!enemyWave[i]->takeDamage(damage))			//Checks if the enemy is killed by the damage
			{	
				enemiesRemaining--;
				state = 1;									// ?
				enemyWave[i]->getGObject()->setAnimationState(2);
			}
				
		}
	}
}

void GameState::playerAttack()
{
	BoundingPolygon hitbox;
	glm::vec2 dirVec = player->getDirection();
	if(player->getWeapon() == SWORD)
	{
		float weaponRange = 5.0f;	//multiplier for the direction vector
		dirVec.x *= weaponRange;
		dirVec.y *= weaponRange;

		Point points[3];
		points[0] = {player->getX(), player->getZ()};
		points[1] = {player->getX() + dirVec.x - dirVec.y, player->getZ() + dirVec.y + dirVec.x};
		points[2] = {player->getX() + dirVec.x + dirVec.y, player->getZ() + dirVec.y - dirVec.x};
		hitbox = BoundingPolygon(points, 3);
	} else													//Using spear
	{
		float weaponRange = 9.0f;
		dirVec.x *= weaponRange;
		dirVec.y *= weaponRange;

		Point points[2];
		points[0] = {player->getX(), player->getZ()};
		points[1] = {player->getX() + dirVec.x, player->getZ() + dirVec.y};
		hitbox = BoundingPolygon(points, 2);
	}
	player->attack();
	for(int i = 0; i < waveSize; i++)
	{
		BoundingPolygon test = enemyWave[i]->getBounds();
		bool hit = test.collides(hitbox);
		if(hit)
		{
			if(enemyWave[i]->isAlive())
			{
				int damage = player->getDamageDealt();
				if(!enemyWave[i]->takeDamage(damage))			//Checks if the enemy is killed by the damage
				{
					enemiesRemaining--;
					state = 1;									// ?
					enemyWave[i]->getGObject()->setAnimationState(2);
					enemyWave[i]->getGObject()->animate(2);
				}
			}
		}
	}
}

int GameState::getState()const
{
	return state;
}

void GameState::loadArena(int fileName)
{
	//Grab from lua
	int error = 0;
	int waveNr = 1;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if(error)
	{
		std::cerr << "Unable to compile errorhandler " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	error = luaL_loadfile(L, "ArenaReader.lua") || lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "errhandler");
	int errhandler = lua_gettop(L);
	if(error)
	{
		std::cerr << "Unable to run 1: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pushinteger(L, fileName);
	lua_setglobal(L, "fileName");
	lua_getglobal(L, "readFile");
	
	error = lua_pcall(L, 0, 2, errhandler);

	if(error)
	{
	//	std::cerr << "Unable to run 2: " << lua_tostring(L, -1) << std::endl;
		std::string luaErr = lua_tostring(L, -1);
		lua_pop(L, 1);
	} else if(!error)
	{
		nrOfArenaObjects = lua_tointeger(L, -1);
		lua_pop(L, 1);
		std::string* arenaArr = new std::string[8 * nrOfArenaObjects];		//0: obj, 1-3: x,y,z, 4: tex-name, 5: tex-index
		int texOffset = render->getTextureSize()-1;
		
		int c = 0;
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			std::string debug = lua_tostring(L, -1);
			arenaArr[c] = debug;
			lua_pop(L, 1);
			c++;
		}
		for(int i = 0; i < nrOfArenaObjects; i++)
		{
			int texIndex = atoi(arenaArr[8*i+5].c_str()) + texOffset;
			if( texIndex >= render->getTextureSize()  ) {
				render->createTexture(arenaArr[8*i + 4]);
			}
			//float x = atoi(arenaArr[6*i+2].c_str());
			GObject* temp = new GObject(arenaArr[8 * i], GL_TRIANGLES, render->getTexture(texIndex));
			temp->translate(atoi(arenaArr[8*i+1].c_str()), atoi(arenaArr[8*i+2].c_str()), atoi(arenaArr[8*i+3].c_str()));
			//temp->rotate(0.0f, -3.14159f / 2.0f, -3.14159f / 2.0f);
			int width = atoi(arenaArr[8 * i + 6].c_str());
			int length = atoi(arenaArr[8 * i + 7].c_str());
			//for(int k = 0; k < width * 2; k++)
			//{
			//	for(int j = 0; j < length * 2; j++)
			//	{
			//		int posX = atoi(arenaArr[8 * i + 1].c_str()) + k - width;
			//		int posZ = atoi(arenaArr[8 * i + 3].c_str()) + j - length;
			//		if(posX >= 0 && posZ >= 0 && posX < GASIZE && posZ < GASIZE)
			//		{
			//			float dist = std::sqrt(pow((float)(k - width), 2) + pow((float)(j - length), 2));
			//			if(k < 3 * width/2 && k > width/2 && j < 3 * length/2 && j > length / 2 )
			//			{
			//				arenaMap[posX][posZ] = -100;
			//			} else
			//			{
			//				arenaMap[posX][posZ] -= (int)(-10.0 / dist);
			//			}
			//		}
			//	}
			//}
			if (i == 0)		//scale the plane
			{
				temp->scale(1.78, 1.0, 1.0);
			}

			if (fileName == 1)
			{
				if (i == 1)//Scalings for map 1
				{
					temp->scale(1.5f, 1.0f, 1.0f);		//left house
				}
				else if (i == 3 || i == 4)	//wall left / right
				{
					temp->scale(1.2f, 1.0f, 1.4f);
				}
				else if (i == 5 || i == 6)	//wall down / up
				{
					temp->scale(1.7f, 1.0f, 1.0f);
				}
				else if (i == 10)
				{
					temp->rotate(0.0f, -3.14159f / -2.0f, 0.0f);	//bottom house
					temp->scale(1.2f, 1.0f, 1.0f);
				}
				else if (i == 11)
				{
					temp->rotate(0.0f, -3.14159f / 1.0f, 0.0f);	//Right house
				}
				else if (i == 12)
				{
					temp->rotate(0.0f, -3.14159f / 2.0f, 0.0f);	//Top house
				}
			}
			else if (fileName == 2)		//scalings for map 2
			{
				if (i < 5 && i != 0)
				{
					temp->scale(0.6f, 0.4f, 0.6f);
				}
				else if (i == 5 || i == 8)	// down / up
				{
					temp->scale(0.7f, 0.4f, 0.5f);
				}
				else if (i == 6 || i == 7)		//right / left
				{
					temp->scale(0.7f, 0.4f, 0.45f);
				}
			}
			else if (fileName == 3)
			{
				if (i < 5 && i != 0)		//Corners
				{
					temp->scale(0.4, 0.4, 0.6);
				}
				else if (i == 5 || i == 6)	//Walls up & down
				{
					temp->scale(0.2, 1.0, 0.9);
				}
				else if (i == 7 || i == 8)	//Walls left & right
				{
					temp->scale(0.9, 1.0, 0.3);
				}
			}

			renderObjects.push_back(temp);

		}
		delete[] arenaArr;
	}
	lua_close(L);
}

void GameState::spawnEnemies(int waveNumber)
{
	//Grab from lua
	int error = 0;
	int waveNr = waveNumber;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	if(error)
	{
		std::cerr << "Unable to compile errorhandler " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	error = luaL_loadfile(L, "enemyreader.lua") || lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "errhandler");
	int errhandler = lua_gettop(L);

	if(error)
	{
		std::cerr << "Unable to run 1: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pushinteger(L, waveNr);
	lua_setglobal(L, "waveNr");
	lua_getglobal(L, "readFile");
	error = lua_pcall(L, 0, 2, errhandler);

	if(error)
	{
		std::string luaErr = lua_tostring(L, -1);
		std::cerr << "Unable to run 2: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	} else if(!error)
	{
		waveSize = lua_tointeger(L, -1);
		lua_pop(L, 1);
		std::string* enemyArgs = new std::string[waveSize * 6];
		int texOffset = render->getTextureSize()-1;
		int c = 0;
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			enemyArgs[c] = lua_tostring(L, -1);
			lua_pop(L, 1);
			c++;
		}
		enemiesRemaining = waveSize;
		enemyWave = new Enemy*[waveSize];
		firstEnemyIndex = renderObjects.size();			//Note: There could possibly be an offset.

		TCHAR NPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, NPath);
		std::wstring wstr(&NPath[0]);
		std::string basePath(wstr.begin(), wstr.end());

		for(int i = 0; i < waveSize; i++)
		{
			int texIndex = atoi(enemyArgs[6*i+3].c_str()) + texOffset;
			if( texIndex >= render->getTextureSize()  ) {
				render->createTexture(enemyArgs[6*i + 2]);
			}
			
			/*checks folder for obj-files*/
			std::string* objArr = new std::string[16];
			WIN32_FIND_DATAA FindFileData;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			std::string str = basePath + "/resource" + enemyArgs[(6 * i) + 1]+ "*.obj";
			hFind = FindFirstFileA(str.c_str(), &FindFileData);
			int c = 0;
			if(hFind != INVALID_HANDLE_VALUE) {
				do {
					objArr[c] = FindFileData.cFileName;
					objArr[c] = enemyArgs[(6 * i) + 1].c_str() + objArr[c];
					c++;
				} while(FindNextFileA(hFind, &FindFileData));
					FindClose(hFind);
			}

			Enemy* tempEnemy = new Enemy(atoi(enemyArgs[6 * i].c_str()), atof(enemyArgs[(6 * i) + 4].c_str()), 
										atof(enemyArgs[(6 * i) + 5].c_str()), render->getTexture(texIndex), objArr, c, waveNumber);
			enemyWave[i] = tempEnemy;
			renderObjects.push_back(tempEnemy->getGObject());
		}
		delete[] enemyArgs;
	}
	//renderObjects.push_back(enemies[i].getLoadObj();
	lua_close(L);
}

void GameState::spawnPlayer()
{
	GLuint textures[3];
	textures[0] = render->createTexture("animations/feet/testtexture.png");
	textures[1] = render->createTexture("animations/spear/mainchartexturespear.png");
	textures[2] = render->createTexture("animations/sword/mainchartexture.png");
	player = new Player(textures, 100, 100, 6, 0);
	
	renderObjects.push_back(player->getGObject(0));
	renderObjects.push_back(player->getGObject(1));
	weaponRender = renderObjects.size() -1;

}

bool GameState::playerCanMove(int x, int z)
{
	BoundingPolygon playerBounds = player->getBounds();
	playerBounds.move(x, z);

	if(playerBounds.findMax({0.0f, 1.0f}) >= GASIZE)
	{
		return false;
	} else if(playerBounds.findMin({0.0f, 1.0f}) <= 0)
	{
		return false;
	} else if(playerBounds.findMax({0.0f, 1.0f}) >= GASIZE)
	{
		return false;
	} else if(playerBounds.findMin({0.0f, 1.0f}) <= 0)
	{
		return false;
	}
	if(arenaMap[(int)player->getZ() + z][(int)player->getX() + x] < 0)
	{
		return false;
	}

	//for(int i = 0; i < waveSize; i++)		//only checks with enemies right now. Need to add environment check
	//{
	//	if(playerBounds.collides(enemyWave[i]->getBounds()))
	//	{
	//		return false;
	//	}
	//}
	return true;
}

void GameState::nextWave()
{
	for (int i = 0; i < waveSize; i++)
	{
		delete enemyWave[i];
		renderObjects.pop_back();
	}

	delete enemyWave;	//Remove last wave
	enemyWave = nullptr;

	if (waveNumber == 6 || waveNumber == 12)
	{
		gold += 30;	//Grant gold for finished boss

		arenaCleanUp();		//Load the next map
 		*currentMap = *currentMap + 1;
   		loadArena(*currentMap);
	}
	else
	{
		gold += 10;		//Grant gold for finished wave
	}
	player->stop(true, true);

	shopUI->setState();	//Show shop
	shopUI->showGold(gold);
	shopUI->setHealingInLua(player);

	waveNumber++;		//Load in next wave
	spawnEnemies(waveNumber);
	if (waveNumber == 6)	//change the boss stats.
	{
		enemyWave[0]->setEnemy(FIRSTBOSS);
	}
	else if (waveNumber == 12)
	{
		enemyWave[0]->setEnemy(SECONDBOSS);
	}
	else if (waveNumber == 18)
	{
		enemyWave[0]->setEnemy(FINALBOSS);
	}

}

int GameState::guiState()
{
	return menuUI->state;
}

int GameState::getShopState()
{
	return shopUI->getState();
}

int GameState::screenClickesOn(float mx, float my)
{
	if (menuUI->state > 2)
	{
		int tmp = menuUI->mouseClick(mx, my);
		if (tmp == 0)
			maxHeal();

		if (tmp == 5 || tmp == 6)
			dead = false;

		return tmp;
	}
	else if (shopUI->getState() == 1)
	{
		int tmp = shopUI->mouseClick(mx, my, gold, player);
		if (tmp == 3)
			gameUI->addHealth();
		else if (tmp == 4)
			maxHeal();
		return tmp;
	}
}

void GameState::maxHeal()
{
  	gameUI->heal(player);
}

void GameState::saveGame()
{
	int error = 0;
	lua_State* L = shopUI->getL();

	lua_getglobal(L, "saveGame");
	lua_pushnumber(L, gold);
	lua_pushnumber(L, *currentMap); //Map

	error = lua_pcall(L, 2, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}

void GameState::saveGameOnDefeat()
{
	int error = 0;
	lua_State* L = shopUI->getL();

	lua_pushcfunction(L, savedGameInfo);
	lua_setglobal(L, "savedGameInfo");

	lua_getglobal(L, "onDefeatSave");
	lua_pushnumber(L, gold);
	lua_pushnumber(L, *currentMap); //Map
	lua_pushnumber(L, waveNumber); //Wave
	lua_pushnumber(L, player->getMaxHealth()/2); //Wave

	error = lua_pcall(L, 4, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}

void GameState::loadSavedGame()
{
	int error = 0;
	lua_State* L = shopUI->getL();

	lua_pushcfunction(L, savedGameInfo);
	lua_setglobal(L, "savedGameInfo");

	lua_getglobal(L, "loadGame");
	lua_pushlightuserdata(L, gameUI);
	lua_pushlightuserdata(L, shopUI);
	lua_pushlightuserdata(L, player);
	lua_pushlightuserdata(L, currentMap);
	lua_pushlightuserdata(L, whichSavedWave);

	error = lua_pcall(L, 5, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	gold = player->getGold();
}

void GameState::loadDefeatSavedGame()
{
	int error = 0;
	lua_State* L = shopUI->getL();

	lua_pushcfunction(L, savedGameInfo);
	lua_setglobal(L, "savedGameInfo");

	lua_getglobal(L, "onDefeatLoad");
	lua_pushlightuserdata(L, gameUI);
	lua_pushlightuserdata(L, shopUI);
	lua_pushlightuserdata(L, player);
	lua_pushlightuserdata(L, currentMap);
	lua_pushlightuserdata(L, whichSavedWave);

	error = lua_pcall(L, 5, 0, 0);
	if (error)
	{
		std::cerr << "Unable to run: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	gold += player->getGold();
}

int GameState::savedGameInfo(lua_State *L) //Called from lua
{
	Player* tmpPlayer;
	InGameGui* tmpGUI;
	ShopUI* tmpShopUI;
	int* tmpMapInt;
	int* tmpWaveInt;

	int ggold = -1;
	int whichMap = -1;
	int whichWave = -1;
	int upgradeSword = -1;
	int upgradeSpear = -1;
	int upgradeHealth = -1;
	int upgradeArmor = -1;
	int nrOfHp = -1;

	tmpPlayer = static_cast<Player*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	tmpGUI = static_cast<InGameGui*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	tmpShopUI = static_cast<ShopUI*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	
	tmpMapInt = static_cast<int*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	tmpWaveInt = static_cast<int*>(lua_touserdata(L, -1));
	lua_pop(L, 1);

	bool defeatn = lua_toboolean(L, -1);
	lua_pop(L, 1);
	ggold = lua_tointeger(L, -1);
	lua_pop(L, 1);
	if (defeatn)
	{
		whichWave = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	whichMap = lua_tointeger(L, -1);
	lua_pop(L, 1);

	upgradeArmor = lua_tointeger(L, -1);
	lua_pop(L, 1);
	upgradeHealth = lua_tointeger(L, -1);
	lua_pop(L, 1);
	upgradeSpear = lua_tointeger(L, -1);
	lua_pop(L, 1);
	upgradeSword = lua_tointeger(L, -1);
	lua_pop(L, 1);
	nrOfHp = lua_tointeger(L, -1);
	lua_pop(L, 1);

	*tmpMapInt = whichMap;
	if (defeatn)
		*tmpWaveInt = whichWave;

	tmpPlayer->setMaxHealth(3 + nrOfHp);
	for (int i = 0; i < nrOfHp; i++)
		tmpGUI->addHealth();

	tmpPlayer->setGold(ggold);
	tmpPlayer->setMaxHealth(6 + nrOfHp*2);
	tmpPlayer->setHealth(tmpPlayer->getMaxHealth());
	tmpPlayer->setArmour(upgradeArmor);
	tmpPlayer->setWeaponUpgrade(1, upgradeSword);
	tmpPlayer->setWeaponUpgrade(2, upgradeSpear);

	tmpShopUI->setSavedGameInfo(upgradeSword, upgradeSpear, upgradeHealth, upgradeArmor);

	tmpShopUI->showGold(ggold);

	tmpPlayer = nullptr;
	tmpGUI = nullptr;
	tmpShopUI = nullptr;
	tmpMapInt = nullptr;
	tmpWaveInt = nullptr;

	return 0;
}
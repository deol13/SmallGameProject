#include "GameState.h"

GameState::GameState( int w, int h)
{
	onExitCleanUp = false;
	//init(w, h);
}

GameState::~GameState()
{
	clean();
}

void GameState::init(int w, int h) 
{
	state = 0;
	//initialize the board the AI uses
	this->board = new int*[GASIZE];
	for (int x = GASIZE - 1; x >= 0; x -- )
	{
		this->board[x] = new int[GASIZE];

		for (int z = GASIZE - 1; z >= 0; z --)
		{
			if (x == GASIZE - 1 || z == GASIZE - 1 || x == 0 || z == 0)
			{
				this->board[x][z] = -10;		//if an edge of the board
			}
			else
			{
				this->board[x][z] = 0;
			}
		}
	}

	enemyWave = nullptr;
	//Set render
	render = new Render(256, w/h);
	render->init(256, w, h);
	//Load Menu UI
	menuUI = new GuiManager(w, h);
	//Load GUI
	gameUI = new InGameGui();
	//Set player
	spawnPlayer();
	//Load arena
	//loadArena("temp");
	//Spawn first enemy wave
	spawnEnemies("placeholder");
	enemiesRemaining = waveSize;

	onExitCleanUp = true;
}

void GameState::clean()
{
	if (onExitCleanUp)
	{
		delete render;
		render = nullptr;
		delete gameUI;
		gameUI = nullptr;
		for (int i = 0; i < waveSize; i++)
		{
			delete enemyWave[i];
		}
		delete[] enemyWave;
		enemyWave = nullptr;

		delete player;

		for (int i = 0; i < GASIZE; i++)
		{
			delete board[i];
		}
		delete[] board;

		renderObjects.clear();

		onExitCleanUp = false;
	}
}

void GameState::update()
{

 	player->update();
	
	if (menuUI->state != 3)
	{
		for (int i = 0; i < waveSize; i++)
		{
			if (enemyWave[i]->getHealth() > 0)
			{
				enemyWave[i]->act(player->getX(), player->getZ(), board);
			}
		}
	}

	render->GeometryPassInit();
	render->render(renderObjects);
	render->lightPass();

	gameUI->update();
	if (menuUI->state == 3)
	{
		menuUI->update();
	}
	//Debug function that drains health of enemy. Currently bugged
	//if(!enemyWave[0]->takeDamage(1))	
	//{
	//	renderObjects.erase(renderObjects.begin() + firstEnemyIndex);
	//	delete enemyWave[0];
	//	enemyWave[0] = enemyWave[--waveSize];
	//}
	//if(waveSize < 1)		//wave has been defeated
	//{
	////	spawnEnemies("altwave.dat");
	//}
}

void GameState::keyDown(char c)
{
	Player::Direction dir;
	bool skipSetDir = false;

	switch (c)
	{
	case 'w':
	case 'W':
		dir = Player::UP;
		break; 
	case 's':
	case 'S':
		dir = Player::DOWN;
		break;
	case 'a':
	case 'A':
		dir = Player::LEFT;
		break;
	case 'd':
	case 'D':
		dir = Player::RIGHT;
		break;
	case 'p':
	case 'P':
		dir = Player::STILL;
		menuUI->pauseGame();
		break;
	case 'q':
	case 'Q':
		skipSetDir = true;
		gameUI->changeWeapon();
		break;
	case 'e': //Temporary
	case 'E': //Temporary
		skipSetDir = true; //Temporary
		gameUI->addHealth(); //Temporary
		break; //Temporary
	case 'r': //Temporary
	case 'R': //Temporary
		skipSetDir = true; //Temporary
		gameUI->DmgTaken(); //Temporary
		break; //Temporary
	case 'f': //Temporary
	case 'F': //Temporary
		skipSetDir = true; //Temporary
		gameUI->heal(); //Temporary
		break; //Temporary
	default: 
		dir = Player::STILL;
		break;
	}
	if (!skipSetDir)
	{
		if (playerCanMove(dir))
		{
			player->setMovement(dir, true);
		}
		else
		{
			player->setMovement(dir, false);
		}
	}
}
void GameState::keyUp(char c)
{
	if ( c == 'w' || c == 'W' ) {
		player->setMovement( Player::UP, false );
	}
	if ( c == 's' || c == 'S' ) {
		player->setMovement( Player::DOWN, false );
	}
	if ( c == 'a' || c == 'A' ) {
		player->setMovement( Player::LEFT, false );
	}
	if ( c == 'd' || c == 'D' ) {
		player->setMovement( Player::RIGHT, false );
	}
}

void GameState::leftMouseClick(long x, long y)
{
	//glm::vec2 vecFromPlayer = glm::vec2(x - player->getX(), y - player->getZ());
	//float angle = atan(vecFromPlayer.y / vecFromPlayer.x);
	//glm::vec2 dirVec =  glm::normalize(glm::vec2(x - player->getX(), y - player->getZ()));
	

	//
	glm::vec2 dirVec = glm::normalize(glm::vec2(x - player->getX(), y - player->getZ()));	//multiply this with weapon size
	BoundingRect hitbox = {player->getX(), player->getZ(), player->getX() + 20 * dirVec.x, player->getZ() + 20 * dirVec.y};
	for(int i = 0; i < waveSize; i++)
	{
		if(enemyWave[i]->getBounds().collides(hitbox))
		{
			if(!enemyWave[i]->takeDamage(10))			//Checks if the enemy is killed by the damage
			{	
				enemiesRemaining--;
				state = 1;
			}
				
		}
	}
}

int GameState::getState()const
{
	return state;
}

void GameState::loadArena(std::string fileName)
{
	//Temporary solution for the ground quad
	//std::vector<Vertex> groundVertices = {
	//	{256.0f, 1.0f, 256.0f, 0.0f, 1.0f},
	//	{256.0f, 1.0f, 0.0f, 0.0f, 0.0f},
	//	{0.0f, 1.0f, 256.0f, 1.0f, 1.0f},
	//	{0.0f, 1.0f, 0.0f, 1.0f, 0.0f}
	//};
	//renderObjects.push_back(new GObject(groundVertices, GL_TRIANGLES, render->getTexture(0)));

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
	lua_pushinteger(L, waveNr);
	lua_setglobal(L, "waveNr");
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
		std::string* arenaArr = new std::string[5 *nrOfArenaObjects];

		int c = 0;
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{

			arenaArr[c] = lua_tostring(L, -1);
			lua_pop(L, 1);
			c++;
		}
		for(int i = 0; i < nrOfArenaObjects; i++)
		{
			float x = atoi(arenaArr[5*i+2].c_str());
			GObject* temp = new GObject(arenaArr[5 * i], GL_TRIANGLES, render->getTexture(atoi(arenaArr[5*i+1].c_str())));
			temp->translate(atoi(arenaArr[5*i+2].c_str()), atoi(arenaArr[5*i+3].c_str()), atoi(arenaArr[5*i+4].c_str()));
			temp->rotate(0.0f, -3.14159f / 2.0f, -3.14159f / 2.0f);
			temp->scale(1.5f, 1.5f, 1.5f);
			renderObjects.push_back(temp);

		}
		delete[] arenaArr;
	}
}

void GameState::spawnEnemies(std::string fileName)
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
		std::cerr << "Unable to run 2: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	} else if(!error)
	{
		waveSize = lua_tointeger(L, -1);
		lua_pop(L, 1);
		float* enemyArgs = new float[waveSize * 4];
		int c = 0;
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			enemyArgs[c] = lua_tonumber(L, -1);
			lua_pop(L, 1);
			c++;
		}
		enemyWave = new Enemy*[waveSize];
		firstEnemyIndex = renderObjects.size();			//Note: There could possibly be an offset.
		for(int i = 0; i < waveSize; i++)
		{
			Enemy* tempEnemy = new Enemy((int)enemyArgs[4 * i], enemyArgs[(4 * i) + 1], enemyArgs[(4 * i) + 2], render->getTexture((int)enemyArgs[(4 * i) + 3]));
			enemyWave[i] = tempEnemy;
			createNegativePotential(enemyWave[i]->getX(), enemyWave[i]->getZ(), 2);	//for AI sets -4 at enemies location
			renderObjects.push_back(tempEnemy->getGObject());
		}
		delete[] enemyArgs;
	}
	//renderObjects.push_back(enemies[i].getLoadObj();
}

void GameState::spawnPlayer()
{
	//Hardcoded for now. Might be worth using lua later
	player = new Player(render->getTexture(1), 100, 100);
	renderObjects.push_back(player->getGObject());

}

bool GameState::playerCanMove(Player::Direction dir)
{
	BoundingRect playerBounds = player->getBounds();

	if(dir == Player::UP)
	{
		if(playerBounds.maxZ >= GASIZE)
		{
			return false;
		} else
		{
			playerBounds.move(0.0f, player->getMoveSpeed());
		}
	}
	if(dir == Player::DOWN)
	{
		if(playerBounds.minZ <= 0)
		{
			return false;
		} else
		{
			playerBounds.move(0.0f, -player->getMoveSpeed());
		}
	}
	if(dir == Player::LEFT)
	{
		if(playerBounds.maxX >= GASIZE)
		{
			return false;
		} else
		{
			playerBounds.move(player->getMoveSpeed(), 0.0f);
		}
	}
	if(dir == Player::RIGHT)
	{
		if(playerBounds.minX <= 0)
		{
			return false;
		} else
		{
			playerBounds.move(-player->getMoveSpeed(), 0.0f);
		}
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

void GameState::createNegativePotential(int posX, int posZ, int size)
{
	board[posZ][posX] += -4;

	int length = 0;
	for (int i = posZ - size; i <= posZ + size; i++)
	{
		for (int j = posX - size; j <= posX + size; j++)
		{
			length = sqrt(((j - posX)*(j - posX)) + ((i - posZ)*(i - posZ)));
			if (length < size)
			{
				if (length > 0)
					board[i][j] -= size - length;
			}
		}
	}
}

int GameState::guiState()
{
	return menuUI->state;
}

int GameState::screenClickesOn(float mx, float my)
{
	return menuUI->mouseClick(mx, my);
}
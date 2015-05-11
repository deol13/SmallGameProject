#include "GameState.h"

GameState::GameState( int w, int h)
{
	onExitCleanUp = false;
	//init(w, h);
	//Set render
	render = new Render(GASIZE, w / h);
	render->init(GASIZE, w, h);
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
	//Load Menu UI
	menuUI = new GuiManager(w, h);
	//Load GUI
	gameUI = new InGameGui();
	//Set player
	spawnPlayer();
	//Load arena
	loadArena("temp");
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
	else
		gameUI->update();
}

void GameState::keyDown(char c)
{
	Player::Direction dir;
	int tmp = 0;
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
		tmp = gameUI->dmgTaken(3); //Temporary
		if (tmp == 1)
			menuUI->defeat();
		break; //Temporary
	case 'f': //Temporary
	case 'F': //Temporary
		skipSetDir = true; //Temporary
		gameUI->heal(false); //Temporary
		break; //Temporary
	case 'z': //Temporary
	case 'Z': //Temporary
		skipSetDir = true; //Temporary
		gameUI->increaseCombo(); //Temporary
		break; //Temporary
	case 'x': //Temporary
	case 'X': //Temporary
		skipSetDir = true; //Temporary
		gameUI->comboLost(); //Temporary
		break; //Temporary
	case 'c': //Temporary
	case 'C': //Temporary
		skipSetDir = true; //Temporary
		menuUI->defeat(); //Temporary
		break; //Temporary
	case 'v': //Temporary
	case 'V': //Temporary
		skipSetDir = true; //Temporary
		menuUI->won(); //Temporary
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
	float weaponSize = 5.0f;	//multiplier for the direction vector
	glm::vec2 dirVec = glm::normalize(glm::vec2(weaponSize*(x - player->getX()), weaponSize*(y - player->getZ())));
	Point points[3];
	points[0] = {player->getX(), player->getZ()};
	points[1] = {player->getX() + dirVec.x - dirVec.y, player->getZ() + dirVec.y + dirVec.x};		
	points[1] = {player->getX() + dirVec.x + dirVec.y, player->getZ() + dirVec.y - dirVec.x};
	BoundingPolygon hitbox = BoundingPolygon(points, 3);
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
		std::string* arenaArr = new std::string[6 * nrOfArenaObjects];					//0: obj, 1-3: x,y,z, 4: tex-name, 5: tex-index
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
			int texIndex = atoi(arenaArr[6*i+5].c_str()) + texOffset;
			if( texIndex >= render->getTextureSize()  ) {
				render->createTexture(arenaArr[6*i + 4]);
			}
			//float x = atoi(arenaArr[6*i+2].c_str());
			GObject* temp = new GObject(arenaArr[6 * i], GL_TRIANGLES, render->getTexture(texIndex));
			temp->translate(atoi(arenaArr[6*i+1].c_str()), atoi(arenaArr[6*i+2].c_str()), atoi(arenaArr[6*i+3].c_str()));
			//temp->rotate(0.0f, -3.14159f / 2.0f, -3.14159f / 2.0f);
			//temp->scale(0.05f, 0.05f, 0.05f);
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
		enemyWave = new Enemy*[waveSize];
		firstEnemyIndex = renderObjects.size();			//Note: There could possibly be an offset.
		for(int i = 0; i < waveSize; i++)
		{
			int texIndex = atoi(enemyArgs[6*i+3].c_str()) + texOffset;
			if( texIndex >= render->getTextureSize()  ) {
				render->createTexture(enemyArgs[6*i + 2]);
			}
			Enemy* tempEnemy = new Enemy(atoi(enemyArgs[6 * i].c_str()), atof(enemyArgs[(6 * i) + 4].c_str()), atof(enemyArgs[(6 * i) + 5].c_str()), render->getTexture(texIndex));
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
	render->createTexture("TestAnimation/testtexture.png");
	player = new Player(render->getTexture(0), 100, 100);
	renderObjects.push_back(player->getGObject());

}

bool GameState::playerCanMove(Player::Direction dir)
{
	BoundingPolygon playerBounds = player->getBounds();

	if(dir == Player::UP)
	{
		if(playerBounds.findMax({0.0f, 1.0f}) >= GASIZE)
		{
			return false;
		} else
		{
			playerBounds.move(0.0f, player->getMoveSpeed());
		}
	}
	if(dir == Player::DOWN)
	{
		if(playerBounds.findMin({0.0f, 1.0f}) <= 0)
		{
			return false;
		} else
		{
			playerBounds.move(0.0f, -player->getMoveSpeed());
		}
	}
	if(dir == Player::LEFT)
	{
		if(playerBounds.findMax({0.0f, 1.0f}) >= GASIZE)
		{
			return false;
		} else
		{
			playerBounds.move(player->getMoveSpeed(), 0.0f);
		}
	}
	if(dir == Player::RIGHT)
	{
		if(playerBounds.findMin({0.0f, 1.0f}) <= 0)
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
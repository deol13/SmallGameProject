#include "GameState.h"

GameState::GameState()
{
	init();
}

void GameState::init() 
{
	enemyWave = nullptr;
	//Set render
	render = new Render(256);
	render->init(256, 640, 480);
	//Load UI
	gameUI = new GuiManager();
	//Set player
	spawnPlayer();
	//Load arena
	//loadArena("temp");
	//Spawn first enemy wave
	spawnEnemies("placeholder");
}

void GameState::clean()
{
	delete render;
	render = nullptr;
	delete gameUI;
	gameUI = nullptr;
	for(int i = 0; i < waveSize; i++)
	{
		delete enemyWave[i];
	}
	delete[] enemyWave;
	enemyWave = nullptr;
}

void GameState::update()
{

	player->update();
	render->GeometryPassInit();
	render->render(gameUI, renderObjects);
	render->lightPass();
}

void GameState::keyDown(char c)
{
	Player::Direction dir;
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
	default:
		dir = Player::STILL;
		break;
	}
	if ( playerCanMove(dir) )
	{
		player->setMovement( dir, true );
	} else 
	{
		player->setMovement( dir, false );
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
	
	GLdouble* worldX;
	GLdouble* worldY;
	GLdouble* worldZ;
	float winDepth;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winDepth);
	GLdouble projMat[16];
	GLdouble viewMat[16];
	GLint viewPort[4]= {0, 0, 640, 480};

	for(int i = 0; i < 16; i++)
	{
		projMat[i] = render->getProjectionMatrix()[i/4][i%4];
		viewMat[i] = render->getViewMatrix()[i / 4][i % 4];
	}
	gluUnProject((double)x, (double)y, winDepth, viewMat, projMat, viewPort, worldX, worldY, worldZ);
	glm::vec2 dirVec = glm::normalize(glm::vec2(*worldX - player->getX(), *worldY - player->getZ()));	//multiply this with weapon size
	for(int i = 0; i < waveSize; i++)
	{
		if(enemyWave[i]->getBounds().collides({player->getX(), player->getZ(), player->getX() + dirVec.x, player->getZ() + dirVec.x}))
		{
			//Enemy is hit
		}
	}
}

void GameState::loadArena(std::string fileName)
{
	//Temporary solution for the ground quad
	std::vector<Vertex> groundVertices = {
		{256.0f, 1.0f, 256.0f, 0.0f, 1.0f},
		{256.0f, 1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 256.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f, 0.0f}
	};
	renderObjects.push_back(new GObject(groundVertices, GL_TRIANGLES, render->getTexture(0)));

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
	error = luaL_loadfile(L, "arenareader.lua") || lua_pcall(L, 0, 0, 0);
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
		nrOfArenaObjects = lua_tointeger(L, -1);
		lua_pop(L, 1);
		float* positions = new float[nrOfArenaObjects * 3];
		std::string* objectFiles = new std::string[nrOfArenaObjects];
		GLuint* textures = new GLuint[nrOfArenaObjects];

		int c = 0;
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			if(c % 5 == 0)
			{
				objectFiles[c] = lua_tostring(L, -1);
			} else if(c % 5 == 1)
			{
				textures[c] = lua_tointeger(L, -1);
			} else
			{
				positions[c] = lua_tonumber(L, -1);
			}
			lua_pop(L, 1);
			c++;
		}
		for(int i = 0; i < nrOfArenaObjects; i++)
		{
			GObject* temp = new GObject(objectFiles[i], GL_TRIANGLES, textures[i]);
			temp->translate(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]);
			renderObjects.push_back(temp);

		}
		delete[] positions;
		delete[] objectFiles;
		delete[] textures;
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

	for(int i = 0; i < waveSize; i++)		//only checks with enemies right now. Need to add environment check
	{
		if(playerBounds.collides(enemyWave[i]->getBounds()))
		{
			return false;
		}
	}
	return true;
}
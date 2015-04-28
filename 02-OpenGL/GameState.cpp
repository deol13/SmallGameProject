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

void GameState::loadArena(std::string fileName)
{
	//Temporary solution for the ground quad
	std::vector<Vertex> groundVertices = {
		Vertex(256.0f, 1.0f, 256.0f, 0.0f, 1.0f), 
		Vertex(256.0f, 1.0f, 0.0f, 0.0f, 0.0f),  
		Vertex(0.0f, 1.0f, 256.0f, 1.0f, 1.0f),
		Vertex(0.0f, 1.0f, 0.0f, 1.0f, 0.0f)
	};
	renderObjects.push_back( new GObject( groundVertices, GL_TRIANGLES, render->getTexture( 0 ) ) );

	/*//Grab from lua
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
	}*/
}

void GameState::spawnEnemies(std::string fileName)
{
	//Grab from lua
	int error = 0;
	int waveNr = 1;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if (error)
	{
		std::cerr << "Unable to compile errorhandler " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	error = luaL_loadfile(L, "enemyreader.lua") || lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "errhandler");
	int errhandler = lua_gettop(L);

	if (error)
	{
		std::cerr << "Unable to run 1: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_pushinteger(L, waveNr);
	lua_setglobal(L, "waveNr");
	lua_getglobal(L, "readFile");
	error = lua_pcall(L, 0, 2, errhandler);

	if (error)
	{
		std::cerr << "Unable to run 2: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	} else if (!error)
	{
		waveSize = lua_tointeger(L, -1);
		lua_pop(L, 1);
		float* enemyArgs = new float[waveSize*4];
		int c = 0;
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			enemyArgs[c] = lua_tonumber(L, -1);
			lua_pop(L, 1);
			c++;
		}
		enemyWave = new Enemy*[waveSize];
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
	player = new Player(render->getTexture(1), 0, 0);
	renderObjects.push_back(player->getGObject());

}

bool GameState::playerCanMove( Player::Direction dir )
{
	BoundingRect playerBounds = player->getBounds();
	switch ( dir )
	{
	case Player::UP:
		if ( playerBounds.maxZ >= GASIZE )
		{
			return false;
		} else
		{
			playerBounds.move( 0.0f, player->getMoveSpeed() );
		}
		break;
	case Player::DOWN:
		if ( playerBounds.minZ <= GASIZE )
		{
			return false;
		} else
		{
			playerBounds.move( 0.0f, -player->getMoveSpeed() );
		}
		break;
	case Player::LEFT:
		if ( playerBounds.maxX >= GASIZE )
		{
			return false;
		} else
		{
			playerBounds.move( player->getMoveSpeed(), 0.0f );
		}
		break;
	case Player::RIGHT:
		if ( playerBounds.minX <= GASIZE )
		{
			return false;
		} else
		{
			playerBounds.move( player->getMoveSpeed(), 0.0f );
		}
		break;
	default:
		return false;
	}
	for ( int i = 0; i < waveSize; i++ )		//only checks with enemies right now. Need to add environment check
	{
		if ( playerBounds.collides( enemyWave[ i ]->getBounds() ) )
		{
			return false;
		}
	}
	return true;
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
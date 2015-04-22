#include "GameState.h"

GameState::GameState()
{
	init();
}

void GameState::init() 
{
	//Set render
	render = new Render(256);
	render->init(256);
	//Load UI
	gameUI = new GuiManager();
	//Set player
	spawnPlayer();
	//Load arena

	//Spawn first enemy wave
	spawnEnemies("placeholder");
}

void GameState::clean()
{
	delete render;
	render = nullptr;
	delete gameUI;
	gameUI = nullptr;
}

void GameState::update()
{

	player->update();
	render->render(gameUI, renderObjects);
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
	renderObjects.push_back( new GObject( groundVertices, GL_TRIANGLE_STRIP, render->getTexture( 0 ) ) );
}

void GameState::spawnEnemies(std::string fileName)
{
	//Grab from lua
}

void GameState::spawnPlayer()
{
	//Hardcoded for now. Might be worth using lua later
	player = new Player(render->getTexture(1), 0, 0);
	renderObjects.push_back(player->getGObject());

}

void GameState::keyDown(char c)
{
	if ( c == 'w' || c == 'W' ) {
		player->setMovement( player->UP, true );
	}
	if ( c == 's' || c == 'S' ) {
		player->setMovement( player->DOWN, true );
	}
	if ( c == 'a' || c == 'A' ) {
		player->setMovement( player->LEFT, true );
	}
	if ( c == 'd' || c == 'D' ) {
		player->setMovement( player->RIGHT, true );
	}
}
void GameState::keyUp(char c)
{
	if ( c == 'w' || c == 'W' ) {
		player->setMovement( player->UP, false );
	}
	if ( c == 's' || c == 'S' ) {
		player->setMovement( player->DOWN, false );
	}
	if ( c == 'a' || c == 'A' ) {
		player->setMovement( player->LEFT, false );
	}
	if ( c == 'd' || c == 'D' ) {
		player->setMovement( player->RIGHT, false );
	}
}
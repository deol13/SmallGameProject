#include "enemyHandler.h"

enemyHandler::enemyHandler()
{
	this->wavesize = 0;
	this->wave = new enemy[10];

}

enemyHandler::enemyHandler(GLuint shaderProgram)
{
	this->shaderProgram = shaderProgram;
	this->wave = nullptr;
}

enemyHandler::~enemyHandler()
{
	delete wave;
}

int enemyHandler::getWaveSize()
{
	return wavesize;
}

void enemyHandler::createWave(int melee, GObject* meleeObj, int ranged, GObject* rangedObj, int tank, GObject* tankObj)
{
	if (wave != nullptr)
	{
		delete wave;
	}

	wavesize = melee + ranged + tank;
	wave = new enemy[wavesize];

	int nrOfEnemies = 0;

	for (int i = 0; i < melee; i++)
	{
		this->wave[i].setEnemy(MELEE);
		this->wave[i].loadObj = meleeObj;
		meleeObj->translate(0, 0, 100.0f);
		meleeObj->translate(100.0f, 0, 0);
		meleeObj->translate(0,1.0f,0);
	}

	nrOfEnemies += melee;

	for (int i = nrOfEnemies; i < nrOfEnemies + ranged; i++)
	{
		this->wave[i].setEnemy(RANGED);
		this->wave[i].loadObj = rangedObj;
	}

	nrOfEnemies += ranged;

	for (int i = nrOfEnemies; i < nrOfEnemies + tank; i++)
	{
		this->wave[i].setEnemy(TANK);
		this->wave[i].loadObj = tankObj;
	}
}

void enemyHandler::clearWave()
{
	delete wave;
}

void enemyHandler::makeMove( float playerX, float playerZ)	//spelarens objekt eller plats
{
	for (int i = 0; i < wavesize; i++)
	{
		wave[i].act(playerX, playerZ);
		wave[i].loadObj->render(uniLocation, shaderProgram);
	}
}


//TMP
void enemyHandler::setUniLoc(GLint uniLocation)
{
	this->uniLocation = uniLocation;
}

void enemyHandler::setShaderPro(GLuint ShaderProgram)
{
	this->shaderProgram = shaderProgram;
}
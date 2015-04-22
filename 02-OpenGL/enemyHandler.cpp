#include "EnemyHandler.h"

EnemyHandler::EnemyHandler()
{
	this->wavesize = 0;
	this->wave = new Enemy[10];

}

EnemyHandler::EnemyHandler(GLuint shaderProgram)
{
	this->shaderProgram = shaderProgram;
	this->wave = nullptr;
}

EnemyHandler::~EnemyHandler()
{
	delete wave;
}

int EnemyHandler::getWaveSize()
{
	return wavesize;
}

void EnemyHandler::createWave(int melee, GObject* meleeObj, int ranged, GObject* rangedObj, int tank, GObject* tankObj)
{
	if (wave != nullptr)
	{
		delete wave;
	}

	wavesize = melee + ranged + tank;
	wave = new Enemy[wavesize];

	int nrOfEnemies = 0;

	for (int i = 0; i < melee; i++)
	{
		this->wave[i].setEnemy(MELEE);
		this->wave[i].loadObj = meleeObj;
		meleeObj->translate(0, 0, 100.0f);
		meleeObj->translate(100.0f, 0, 0);
		meleeObj->translate(0,12.0f,0);
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

void EnemyHandler::clearWave()
{
	delete wave;
}

void EnemyHandler::makeMove( float playerX, float playerZ)	//spelarens objekt eller plats
{
	for (int i = 0; i < wavesize; i++)
	{
		wave[i].act(playerX, playerZ);
		wave[i].loadObj->render(uniLocation, shaderProgram);
	}
}


//TMP
void EnemyHandler::setUniLoc(GLint uniLocation)
{
	this->uniLocation = uniLocation;
}

void EnemyHandler::setShaderPro(GLuint ShaderProgram)
{
	this->shaderProgram = shaderProgram;
}
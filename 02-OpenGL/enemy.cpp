#include "enemy.h"

enemy::enemy()
{
	health = 20;
	type = MELEE;
	moveSpeed = 0.8f;
	posX = 100.0f;
	posZ = 100.0f;
}

enemy::~enemy()
{}

void enemy::setEnemy(int type)
{
	if (type == MELEE)
	{
		health = 20;
		this->type = MELEE;
	}
	else if (type == RANGED)
	{
		health = 10;
		this->type = RANGED;
	}
	else
	{
		health = 40;
		this->type = TANK;
	}

}

void enemy::act(float playerX, float playerZ) //spelarens objekt eller plats
{
	if (type == MELEE)
	{
		posX -= moveSpeed;
		loadObj->translate(moveSpeed, 0, 0);

		posZ += moveSpeed;
		loadObj->translate(0, 0, moveSpeed);
	}
	else if (type == RANGED)
	{

	}
	else if (type == TANK)
	{

	}
}
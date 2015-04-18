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
		
		float moveX = playerX - posX;
		float moveZ = playerZ - posZ;

		if (abs(moveX) + abs(moveZ) > 0.0000001f)
		{
			posX += moveSpeed* moveX / abs(moveX + moveZ);
			posZ += moveSpeed* moveZ / abs(moveX + moveZ);
			loadObj->translate(moveSpeed* moveX / abs(moveX + moveZ), 0, moveSpeed* moveZ / abs(moveX + moveZ));

		}
	}
	else if (type == RANGED)
	{

	}
	else if (type == TANK)
	{

	}
}
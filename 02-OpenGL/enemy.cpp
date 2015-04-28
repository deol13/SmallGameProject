#include "Enemy.h"

Enemy::Enemy()
{
	loadObj = new GObject();
	health = 20;
	type = MELEE;
	moveSpeed = 0.8f;
	this->x = 100.0f;
	this->z = 100.0f;
}

Enemy::Enemy(int type, float x, float z, GLuint texture)
{
	this->x = x;
	this->z = z;
	loadObj = nullptr;

	switch ( type )
	{
	case MELEE:
		loadObj = new GObject( "enamie.obj", GL_TRIANGLES, texture );
		health = 20;
		this->type = MELEE;
		moveSpeed = 0.8f;	
		break;
	default:
		break;
	}
	loadObj->translate(x, 17, z);
}

Enemy::~Enemy()
{
	delete loadObj;
}

void Enemy::setEnemy(int type)
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

void Enemy::act(float playerX, float playerZ) //spelarens objekt eller plats
{
	if (type == MELEE)
	{
		
		float moveX = playerX - x;
		float moveZ = playerZ - z;

		if (abs(moveX) + abs(moveZ) > 0.0000001f)
		{
			x += moveSpeed* moveX / abs(moveX + moveZ);
			z += moveSpeed* moveZ / abs(moveX + moveZ);
			loadObj->translate(moveSpeed* moveX / abs(moveX + moveZ), 0, moveSpeed* moveZ / abs(moveX + moveZ));

		}
		else
		{

		}
	}
	else if (type == RANGED)
	{

	}
	else if (type == TANK)
	{

	}
}

GObject* Enemy::getGObject() const 
{
	return loadObj;
}

BoundingRect Enemy::getBounds() const 
{
	return collisionRect;
}
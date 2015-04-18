#include "Player.h"


Player::Player()
{
	loadObj = new GObject();
	x = 0.0f;
	z = 0.0f;
	moveSpeed = 1;
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}
Player::Player(GLuint texture, float x, float z)
{
	loadObj = new GObject("Victest.obj", GL_QUADS, texture);
	this->x = x;
	this->z = z;
	moveSpeed = 0.9;
	loadObj->translate(0,17,0);
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}
void Player::attack()
{}

void Player::setMovement(int dir, bool isMoving)
{
	movement[dir] = isMoving;
}

void Player::takeDamage(int dmg)
{
	health -= dmg;
	if (health <= 0)
	{

	}
}

GObject* Player::getGObject() const
{
	return loadObj;
}

void Player::update()
{
	if (movement[0]) //UP
	{
		z += moveSpeed;
		loadObj->translate(0,0,moveSpeed);
	}
	if (movement[1]) //DOWN
	{
		z -= moveSpeed;
		loadObj->translate(0, 0, -moveSpeed);
	}
	if (movement[2]) //LEFT
	{
		x += moveSpeed;
		loadObj->translate(moveSpeed, 0, 0);
	}
	if (movement[3]) //RIGHT
	{
		x -= moveSpeed;
		loadObj->translate(-moveSpeed, 0, 0);
	}
}


float Player::getX()
{
	return x;
}
float Player::getZ()
{
	return z;
}

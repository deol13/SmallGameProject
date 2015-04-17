#include "Player.h"


Player::Player()
{
	loadObj = new GObject();
	x = 0.0f;
	z = 0.0f;
	moveSpeed = 1;
}
Player::Player(GLuint texture, float x, float z)
{
	loadObj = new GObject("Victest.obj", GL_QUADS, texture);
	this->x = x;
	this->z = z;
	moveSpeed = 1;
}
void Player::attack()
{}

void Player::move(Direction dir)
{
	switch (dir)
	{
	case UP:
		z += moveSpeed;
		loadObj->translate(0, 0, moveSpeed);
		break;
	case DOWN:
		z -= moveSpeed;
		loadObj->translate(0, 0, -moveSpeed);
		break;
	case LEFT:
		x -= moveSpeed;
		loadObj->translate(moveSpeed, 0, 0);
		break;
	case RIGHT:
		x += moveSpeed;
		loadObj->translate(-moveSpeed, 0, 0);
		break;
	default:
		break;
	}
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

//void Player::update()
//{
//	loadObj->render();
//}


float Player::getX()
{
	return x;
}
float Player::getZ()
{
	return z;
}

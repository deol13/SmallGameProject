#include "Player.h"

Player::Player()
{
	loadObj = new GObject*[1];		//change to 3
	loadObj[0] = new GObject();
	x = 5.0f;
	z = 5.0f;
	Point* colPoints = new Point[4]{{0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}, {10.0, 10.0}};
	BoundingPolygon collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 1;
	health = 6;
	armour = 0;
	gold = 0;
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}
Player::Player(GLuint texture, float x, float z, int health, int armour)
{
	std::string files[] = {"TestAnimation/StartFrame.obj", "TestAnimation/Frame10.obj", "TestAnimation/Frame20.obj"};
	loadObj = new GObject*[1];
	loadObj[0] = new GObject(files, 3, texture);
	this->x = x;
	this->z = z;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 0.9;
	this->health = health;
	this->armour = armour;
	this->gold = 0;

	for(int i = 0; i < 1; i++)		//again, change to 3
	{
		loadObj[i]->translate(x, 17, z);
	}
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}

Player::~Player()
{
	for(int i = 0; i < 1; i++)
	{
		delete loadObj[i];
	}
	delete[] loadObj;
}

void Player::attack()
{}

void Player::setMovement(int dir, bool isMoving)
{
	movement[dir] = isMoving;
}

bool Player::takeDamage(const int dmg)
{
	health -= dmg;
	return (health > 0);
}

GObject** Player::getGObjects() const
{
	return loadObj;
}

void Player::update()
{
	float zMove = 0.0;
	float xMove = 0.0;

	if (movement[0]) //UP
	{
		zMove += moveSpeed;
	}
	if (movement[1]) //DOWN
	{
		zMove -= moveSpeed;
	}
	if (movement[2]) //LEFT
	{
		xMove += moveSpeed;
	}
	if (movement[3]) //RIGHT
	{
		xMove -= moveSpeed;
	}

	if(abs(xMove)< 0.001 && abs(zMove)< 0.001)
	{	
		for(int i = 0; i < 1; i++)
		{
			loadObj[i]->setAnimationState(0.0);
		}
	}
	for(int i = 0; i < 1; i++)
	{
		loadObj[i]->translate(xMove, 0, zMove);
		loadObj[i]->animate();
	}
	x += xMove;
	z += zMove;
	collisionRect.move(xMove, zMove);
}

void Player::setGold(const int gold)
{
	this->gold = gold;
}
void Player::setMaxHealth(const int health)
{
	this->maxHealth = health;
}
void Player::setHealth(const int health)
{
	this->health = health;
}
void Player::setArmour(const int armour)
{
	this->armour = armour;
}

float Player::getX() const
{
	return x;
}

float Player::getZ() const
{
	return z;
}

float Player::getAngle() const 
{
	return angle;
}

BoundingPolygon Player::getBounds() const 
{
	return collisionRect;
}

float Player::getMoveSpeed() const
{
	return moveSpeed;
}
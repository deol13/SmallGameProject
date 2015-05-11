#include "Player.h"

Player::Player()
{
	loadObj = new GObject();
	x = 5.0f;
	z = 5.0f;
	Point* colPoints = new Point[4]{{0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}, {10.0, 10.0}};
	BoundingPolygon collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 1;
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}
Player::Player(GLuint texture, float x, float z)
{
	//std::string files[] = {"WalkingAnimation/StandStill.obj", "WalkingAnimation/10frames.obj", "WalkingAnimation/10framesleft.obj"};
	std::string files[] = {"TestAnimation/StartFrame.obj", "TestAnimation/Frame10.obj", "TestAnimation/Frame20.obj"};
	loadObj = new GObject(files, 3, texture);
	this->x = x;
	this->z = z;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 0.9;
	loadObj->translate(x,17,z);
	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}

Player::~Player()
{
	delete loadObj;
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

GObject* Player::getGObject() const
{
	return loadObj;
}

void Player::update()
{
	int moving = 0;
	if (movement[0]) //UP
	{
		z += moveSpeed;
		loadObj->translate(0,0,moveSpeed);
		collisionRect.move( 0, moveSpeed );
		moving++;
	}
	if (movement[1]) //DOWN
	{
		z -= moveSpeed;
		loadObj->translate(0, 0, -moveSpeed);
		collisionRect.move( 0, -moveSpeed );
		moving++;
	}
	if (movement[2]) //LEFT
	{
		x += moveSpeed;
		loadObj->translate(moveSpeed, 0, 0);
		collisionRect.move( moveSpeed, 0 );
		moving++;
	}
	if (movement[3]) //RIGHT
	{
		x -= moveSpeed;
		loadObj->translate(-moveSpeed, 0, 0);
		collisionRect.move( -moveSpeed, 0 );
		moving++;
	}
	if(moving == 0)
	{	
		loadObj->setAnimationState(0.0);
	}
	loadObj->animate();
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
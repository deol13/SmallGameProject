#include "Player.h"

Player::Player()
{
	loadObj = new GObject();
	x = 5.0f;
	z = 5.0f;
	Point* colPoints = new Point[4]{{0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}, {10.0, 10.0}};
	BoundingPolygon collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 1;
	health = 6;
	armour = 0;
	gold = 0;
	this->weapon = SWORD;
	this->spearUpgrade = 1;
	this->swordUpgrade = 1;

	for (int i = 0; i < 4; i++)
	{
		movement[i] = false;
	}
}
Player::Player(GLuint texture, float x, float z, int health, int armour)
{
	//std::string files[] = {"WalkingAnimation/StandStill.obj", "WalkingAnimation/10frames.obj", "WalkingAnimation/10framesleft.obj"};
	std::string files[] = {"TestAnimation/StartFrame.obj", "TestAnimation/Frame10.obj", "TestAnimation/Frame20.obj"};
	loadObj = new GObject(files, 3, texture);
	this->x = x;
	this->z = z;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 0.9;
	this->health = health;
	this->armour = armour;
	this->gold = 0;
	this->weapon = SWORD;
	this->spearUpgrade = 1;
	this->swordUpgrade = 1;


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
void Player::setWeapon(const int weapon)
{
	this->weapon = weapon;
}
void Player::setWeaponUpgrade(const int weapon, const int weaponLevel)
{
	if (weapon == SWORD)
	{
		swordUpgrade = weaponLevel;
	}
	if (weapon == SPEAR)
	{
		spearUpgrade = weaponLevel;

	}
}

int Player::getDamageDealt()
{
	int damage = 0;

	if (weapon == SWORD)
	{
		damage = 7 + (3 * swordUpgrade);
	}

	if (weapon == SPEAR)
	{
		damage = 5 + (2 * spearUpgrade);
	}

	return damage;
}

int Player::getWeapon()
{
	return this->weapon;
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
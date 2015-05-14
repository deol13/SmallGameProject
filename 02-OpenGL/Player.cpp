#include "Player.h"

Player::Player()
{
	invulTimer = 0;
	loadObj = new GObject*[1];		//change to 3
	loadObj[0] = new GObject();
	x = 5.0f;
	z = 5.0f;
	Point* colPoints = new Point[4]{{0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}, {10.0, 10.0}};
	BoundingPolygon collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 1;
	health = 6;
	armour = 0;
	this->weapon = SWORD;
	this->spearUpgrade = 1;
	this->swordUpgrade = 1;
	this->dirVec = glm::vec2(0.0, 0.0);
}
Player::Player(GLuint texture, float x, float z, int health, int armour)
{
	std::string files[] = {"TestAnimation/StartFrame.obj", "TestAnimation/Frame10.obj", "TestAnimation/Frame20.obj"};
	loadObj = new GObject*[1];
	loadObj[0] = new GObject(files, 3, texture);
	this->x = x;
	this->z = z;
	invulTimer = 0;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 0.9;
	this->health = health;
	this->armour = armour;
	this->weapon = SWORD;
	this->spearUpgrade = 1;
	this->swordUpgrade = 1;
	this->dirVec = glm::vec2(0.0, 0.0);

	for(int i = 0; i < 1; i++)		//again, change to 3
	{
		loadObj[i]->translate(x, 17, z);
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

void Player::setMovement(int x, int y)
{
	dirVec.x += x;
	dirVec.y += y;
	if(dirVec.x > 1)
	{
		dirVec.x = 1;
	}
	if(dirVec.y > 1)
	{
		dirVec.y = 1;
	}
	if(dirVec.x < -1)
	{
		dirVec.x = -1;
	}
	if(dirVec.y < -1)
	{
		dirVec.y = -1;
	}
}

void Player::stop(bool stopX, bool stopZ)
{
	if(stopX)
	{
		dirVec.x = 0;
	}
	if(stopZ)
	{
		dirVec.y = 0;
	}
}

bool Player::takeDamage(const int dmg)
{
	if (invulTimer == 0)
	{
		invulTimer = 1;
		health -= dmg;
	}
	return (health > 0);
}

GObject** Player::getGObjects() const
{
	return loadObj;
}

void Player::update()
{

	float xMove = moveSpeed * dirVec.x;
	float zMove = moveSpeed * dirVec.y;

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

	if (invulTimer > 0 && invulTimer < 50)
	{
		invulTimer++;
	}
	else if (invulTimer != 0)
	{
		invulTimer = 0;
	}
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

void Player::setGold(const int gold)
{
	this->gold = gold;
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

int Player::getHealth()
{
	return this->health;
}

int Player::getInvulTimer()
{
	return invulTimer;
}

float Player::getX() const
{
	return x;
}

float Player::getZ() const
{
	return z;
}

glm::vec2 Player::getDirection() const 
{
	return dirVec;
}

BoundingPolygon Player::getBounds() const 
{
	return collisionRect;
}

float Player::getMoveSpeed() const
{
	return moveSpeed;
}

int Player::getGold() const
{
	return gold;
}
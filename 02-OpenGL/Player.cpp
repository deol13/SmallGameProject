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
	maxHealth = health;
	armour = 0;
	this->weapon = SWORD;
	this->spearUpgrade = 1;
	this->swordUpgrade = 1;
	this->dirVec = glm::vec2(0.0, 1.0);
	this->moveVec = glm::vec2(0.0, 0.0);
	this->attackState = 0;
}
Player::Player(GLuint textures[3], float x, float z, int health, int armour)
{
	std::string folder = "animations/feet/";
	std::string legFiles[] = {folder + "footup.obj", folder + "rightup.obj", folder + "stop.obj"};
	
	folder = "animations/sword/";
	std::string swordFiles[] =
	{folder + "swordrun1.obj", folder + "swordrun2.obj", folder + "swordstop.obj",
	folder + "swordstrike1.obj", folder + "swordstrike2.obj"};

	folder = "animations/spear/";
	std::string spearFiles[] = 
		{folder + "spearrun1.obj", folder + "spearrun2.obj", folder + "spearstop.obj",
		folder + "spear1.obj"};

	loadObj = new GObject*[3];
	loadObj[0] = new GObject(legFiles, 3, textures[0]);
	loadObj[1] = new GObject(swordFiles, 5, textures[1]);
	loadObj[2] = new GObject(spearFiles, 4, textures[2]);

	this->x = x;
	this->z = z;
	invulTimer = 0;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 0.9;
	this->health = health;
	maxHealth = health;
	this->armour = armour;
	this->weapon = SWORD;
	this->spearUpgrade = 0;
	this->swordUpgrade = 0;
	this->dirVec = glm::vec2(0.0, 1.0);
	this->moveVec = glm::vec2(0.0, 0.0);
	this->attackState = 0;

	for(int i = 0; i < 3; i++)		//again, change to 3
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
	if(x != 0)
	{
		moveVec.x = x;
	}
	if(y != 0)
	{
		moveVec.y = y;
	}
	
	if(abs(moveVec.x) + abs(moveVec.y) > 1.9)			// Should be 2. The decimal's just a float precaution
	{
		moveVec.x = (moveVec.x / abs(moveVec.x)) * sqrt(2.0) / 2.0;
		moveVec.y = (moveVec.y / abs(moveVec.y)) * sqrt(2.0) / 2.0;
	}
	if(abs(moveVec.x) + abs(moveVec.y) > 0.0)
	{
		dirVec = moveVec;
	}
}

void Player::stop(bool stopX, bool stopZ)
{
	if(stopX)
	{
		moveVec.x = 0;
	}
	if(stopZ)
	{
		moveVec.y = 0;
	}
}

int Player::takeDamage(int dmg)
{
 	if (invulTimer == 0) 
	{
		invulTimer = 1;
		
		dmg -= armour;		//Take away damage equal to the armour
		
		if (dmg <= 0)		//Not negating all damage
		{
			dmg = 1;
		}
		health -= dmg;
	}
	return dmg;
}

GObject* Player::getGObject(int index) const
{
	return loadObj[index];
}

void Player::update()
{
	if(abs(moveVec.x) + abs(moveVec.y) > 0.0)
	{
		dirVec = moveVec;
	}
	float xMove = moveSpeed * moveVec.x;
	float zMove = moveSpeed * moveVec.y;

	if(abs(xMove)< 0.001 && abs(zMove)< 0.001)
	{	
		loadObj[0]->setAnimationState(2.0);
		if(attackState == 0)
		{
			loadObj[weapon]->setAnimationState(2.0);
		}
	}
	loadObj[0]->animate(2);
	if(attackState == 0)
	{
		loadObj[weapon]->animate(2);
	} else
	{
		loadObj[weapon]->animate(5 - weapon);
	}
	for(int i = 0; i < 3; i++)
	{
		loadObj[i]->translate(xMove, 0, zMove);
		/* rotation default is facing up. Turning goes counter clockwise*/
		float angle = 0;
		if(dirVec.y < 0.1)
		{
			//angle = 3.14159;
		} else
		{
			//angle = dirVec.x * (dirVec.y +2) * 3.14159 / 4.0;
		}
		loadObj[i]->setRotation(0, angle, 0);
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
	if( attackState > 0 ) {
		attackState--;
	}
}

void Player::attack()
{
	if(weapon == SWORD)
	{
		//loadObj[2]->animate(5);
		loadObj[1]->setAnimationState(2);
		loadObj[1]->animate(4);
		attackState = 30;
	} else
	{
		loadObj[1]->setAnimationState(2);
		loadObj[1]->animate(3);
		attackState = 20;
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

void Player::setDefault()
{
	x = 128.0;
	z = 128.0;
	stop(true, true);
}

int Player::getDamageDealt()
{
	int damage = 0;

	if (weapon == SWORD)
	{
		damage = 7 + (5 * swordUpgrade) + 9999;
	}

	if (weapon == SPEAR)
	{
		damage = 5 + (4 * spearUpgrade);
	}

	return damage;
}

int Player::getWeapon()
{
	return this->weapon;
}

int Player::getWeaponUpgrade(int weaponChosen)
{
	int upgrade = 0;

	if (weaponChosen == SWORD)
	{
		upgrade = swordUpgrade;
	}
	else
	{
		upgrade = spearUpgrade;
	}
	return upgrade;
}

int Player::getMaxHealth()
{
	return maxHealth;
}

int Player::getHealth()
{
	return this->health;
}

int Player::getArmour()
{
	return armour;
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
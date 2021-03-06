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
	std::string legFiles[] = {folder + "benleft.obj", folder + "benright.obj", folder + "benstop.obj"};
	
	folder = "animations/sword/";
	std::string swordFiles[] =
	{folder + "swordrun1.obj", folder + "swordrun2.obj", folder + "swordstop.obj",
	folder + "swordstrike1.obj", folder + "swordstrike2.obj"};

	folder = "animations/spear/";
	std::string spearFiles[] = 
		{folder + "spearrun1.obj", folder + "spearrun2.obj", folder + "spearstop.obj",
		folder + "spearstrike1.obj"};

	loadObj = new GObject*[3];
	loadObj[0] = new GObject(legFiles, 3, textures[0]);
	loadObj[1] = new GObject(swordFiles, 5, textures[1]);
	loadObj[2] = new GObject(spearFiles, 4, textures[2]);

	this->x = x;
	this->z = z;
	invulTimer = 0;
	Point colPoints[4] ={{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	moveSpeed = 1.0;
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
		loadObj[i]->translate(x, 9, z);
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
	if(abs(moveVec.x) + abs(moveVec.y) > 0.01)
	{
		dirVec = moveVec;
	}

	for(int i = 0; i < 3; i++)
	{
		int roundX = floor(dirVec.x + 0.3);
		int roundY = floor(dirVec.y + 0.3);
		if(roundX == 0)
		{
			loadObj[i]->setRotation(0.0, 3.14159 / 2.0 - roundY * 3.14159 / 2.0, 0.0);
		} else if(roundX == 1)
		{
			loadObj[i]->setRotation(0.0, 3.14159 / 2.0 - (roundY * 3.14159 / 4.0), 0.0);
		} else
		{
			loadObj[i]->setRotation(0.0, -3.14159 / 2.0 + (roundY * 3.14159 / 4.0), 0.0);
		}
		/*if(roundX == 1 && roundY == 1)
		{
			loadObj[i]->setRotation(0.0, (3.14159 / 4.0) + (roundY * 3.14159 / 4.0), 0.0);
		} else if(roundX == -1 && roundY == -1)
		{
			loadObj[i]->setRotation(0.0, -3 * (3.14159 / 4.0) + (roundY * 3.14159 / 4.0), 0.0);
		}*/
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

void Player::update(const float board[455][256])
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

	/*Check that movement isn't blocked*/
	int canMoveX = 1;
	int canMoveZ = 1;
	for(int i = collisionRect.findMin({1.0f, 0.0f}) + xMove; i < collisionRect.findMax({1.0f, 0.0f}) + xMove; i++)
	{
		if(board[i][(int)(collisionRect.findMax({0.0f, 1.0f}) + 0.5 + zMove)] < 0 ||
			board[i][(int)(collisionRect.findMin({0.0f, 1.0f}) - 0.5 + zMove)] < 0)
		{
			canMoveX = -1;
			break;
		}
	}
	for(int i = collisionRect.findMin({0.0f, 1.0f}) + zMove; i < collisionRect.findMax({0.0f, 1.0f}) + zMove; i++)
	{
		int temp1 = (int)(collisionRect.findMax({1.0f, 0.0f}) + 0.5 + xMove);
		int temp2 = (int)(collisionRect.findMin({1.0f, 0.0f}) - 0.5 + xMove);
		if(board[temp1][i] < 0 || board[temp2][i] < 0)
		{
			canMoveZ = -1;
			break;
		}
	}

	xMove *= canMoveX;
	zMove *= canMoveZ;
	for(int i = 0; i < 3; i++)
	{
		loadObj[i]->translate(xMove, 0, zMove);
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
	stop(true, true);
}

int Player::getDamageDealt()
{
	if(attackState > 0)
	{
		return 0;
	}
	int damage = 0;

	if (weapon == SWORD)
	{
		damage = 10 + (5 * swordUpgrade);
	}

	if (weapon == SPEAR)
	{
		damage = 10 + (4 * spearUpgrade);
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
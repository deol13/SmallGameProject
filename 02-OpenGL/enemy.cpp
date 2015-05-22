#include "Enemy.h"

Enemy::Enemy()
{
	loadObj = new GObject();
	health = 20;
	type = MELEE;
	moveSpeed = 0.5f;
	this->x = 100.0f;
	this->z = 100.0f;
	Point colPoints [4] = {{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	idleTimer = 0;

}

Enemy::Enemy(int type, float x, float z, GLuint texture, string objectFile, int waveNr)
{
	this->x = x;
	this->z = z;
	loadObj = nullptr;
	Point* colPoints = new Point[4]{{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	switch ( type )
	{
	case MELEE:
		loadObj = new GObject(objectFile, GL_TRIANGLES, texture);
		health = 20 + 1 * waveNr;
		this->type = MELEE;
		moveSpeed = 0.5f;
		this->attackRange = MELEERANGE;
		this->loadObj->scale(2.0f, 2.0f, 1.5f);
		break;
	case ANIMAL:
		loadObj = new GObject(objectFile, GL_TRIANGLES, texture);
		health = 30 + 2 * waveNr;
		this->type = ANIMAL;
		moveSpeed = 0.6f;
		this->attackRange = ANIMALRANGE;
		this->loadObj->scale(1.5f, 1.0f, 2.0f);
		break;
	default:
		break;
	}
	loadObj->translate(x, 17, z);
	for(int i = 0; i < 8; i++)
	{
		neighbourPos[i] = 0;
	}
}

Enemy::Enemy(int type, float x, float z, GLuint texture, string* objectFiles, int nrOfKeyFrames, int waveNr, int idle)
{
	this->x = x;
	this->z = z;
	charging = false;
	chargeTimer = 0;
	idleTimer = idle;
	loadObj = nullptr;
	Point* colPoints = new Point[4]{{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);
	loadObj = new GObject(objectFiles, nrOfKeyFrames, texture);
	switch(type)
	{
	case MELEE:
		health = 20 + 1 * waveNr;
		this->type = MELEE;
		moveSpeed = 0.5f;
		this->attackRange = MELEERANGE;
		this->loadObj->scale(3.0f, 3.0f, 2.5f);
		break;
	case ANIMAL:
		health = 30 + 2 * waveNr;
		this->type = ANIMAL;
		moveSpeed = 0.6f;
		this->attackRange = ANIMALRANGE;
		this->loadObj->scale(2.5f, 1.0f, 1.8f);
		break;
	case FIRSTBOSS:
		health = 60;
		this->type = FIRSTBOSS;
		moveSpeed = 0.5f;
		chargeTimer = 360;
		this->attackRange = 7;
		this->loadObj->scale(1.5f, 1.0f, 1.5f);
		break;
	default:
		break;
	}
	loadObj->translate(x, 17, z);
	for(int i = 0; i < 8; i++)
	{
		neighbourPos[i] = 0;
	}
	if(z < 70)
	{
		loadObj->rotate(0, 3.14159, 0);
	} else if(x < 100)
	{
		loadObj->rotate(0, -3.14159/2, 0);
	} else if(x > 350)
	{
		loadObj->rotate(0, 3.14159 / 2, 0);
	}
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
	else if (type == ANIMAL)
	{
		health = 40;
		this->type = ANIMAL;
	}
	else if (type == FIRSTBOSS)		//Lion
	{
		health = 60;
		this->type = type;
		loadObj->scale(4.0f, 2.0f, 4.0f);
	}
	else if (type == SECONDBOSS)	//Elephant
	{
		health = 80;
		moveSpeed = 0.2;
		this->type = type;
		loadObj->scale(0.5f, 0.5f, 0.5f);
	}
	else		//final boss
	{
		health = 120;
		moveSpeed = 0.3;
		this->type = type;
		loadObj->scale(0.75f, 1.0f, 0.75f);
	}

}

GObject* Enemy::getGObject() const
{
	return loadObj;
}

float Enemy::getX() const
{
	return this->x;
}
float Enemy::getZ() const
{
	return this->z;
}

int Enemy::getHealth() const
{
	return this->health;
}

int Enemy::getRange() const
{
	return this->attackRange;
}

int Enemy::getType()const
{
	return this->type;
}

BoundingPolygon Enemy::getBounds() const
{
	return collisionRect;
}

bool Enemy::takeDamage(const int dmg, const float playerX, const float playerZ)
{
	if(dmg > 0)
	{
		float dist = sqrt(pow(playerX - x, 2) + pow(playerZ - z, 2));
		float xMove = 15.0f * (x - playerX) / dist;
		float zMove = 15.0f * (z - playerZ) / dist;
		this->x += xMove;
		this->z += zMove;
		loadObj->translate(xMove, 0.0, zMove);
		collisionRect.move(xMove, zMove);

		health -= dmg;
	}
	return (health > 0);
}

bool Enemy::isAlive()const
{
	return (health > 0);
}

bool Enemy::isCharging()const
{
	return charging;
}

bool Enemy::isIdle()const
{
	return idleTimer > 100;
}

bool Enemy::isTangible()const
{
	return idleTimer <= 0;
}

void Enemy::changeIdle()
{
	if(idleTimer > 0)
	{
		idleTimer--;
	}
}

void Enemy::updateCharge()
{
	chargeTimer--;
	if(chargeTimer == 0)
	{
		charging = !charging;
		switch(type)
		{
		case FIRSTBOSS:
			if(charging)
			{
				chargeTimer = 180;
				moveSpeed *= 2;
			} else
			{
				chargeTimer = 360;
				moveSpeed *= 0.5;
			}
			break;
		default:
			break;
		}
	}
}

int Enemy::attack()
{
	int damage = 0;
	if (type == MELEE)
	{
		damage = 1;
	}
	else if (type == RANGED)
	{

	}
	else if (type == ANIMAL)
	{
		damage = 2;
	}
	else if (type == FIRSTBOSS)
	{
		damage = 3;
	}
	else if (type == SECONDBOSS)
	{
		damage = 4;
	}
	else				//Final boss
	{
		damage = 5;
	}
	return damage;
}

void Enemy::move()
{
	int highIndex = 0;
	for(int i = 1; i < 8; i++)
	{
		if(neighbourPos[i] > neighbourPos[highIndex])
		{
			highIndex = i;
		}
	}
	if(neighbourPos[highIndex] > 0)
	{
		float xMove;
		float zMove;
		xMove = ((highIndex + highIndex / 4) % 3 - 1) * moveSpeed;
		zMove = ((highIndex + highIndex / 4) / 3 - 1) * moveSpeed;
		this->x += xMove;
		this->z += zMove;
		loadObj->translate(xMove, 0.0, zMove);
		collisionRect.move(xMove, zMove);
		loadObj->animate(1);
		if(highIndex > 4)
		{
			loadObj->setRotation(0.0, (highIndex - 2)*3.14159 / 4, 0.0);
		} else if(highIndex < 3)
		{
			loadObj->setRotation(0.0, (1 - highIndex)*3.14159 / 4, 0.0);
		} else
		{
			loadObj->setRotation(0.0, (-2 * highIndex + 7)* 3.14159 / 2, 0.0);
		}
	}

	//loadObj->animate(1);
}

void Enemy::setPotential(int origX, int origZ, int basePower)
{
	//float radii[9];
	if(origX > 455)
	{
		origX = 455;
	} else if(origX < 0)
	{
		origX = 0;
	}
	if(origZ > 256)
	{
		origZ = 256;
	} else if(origX < 0)
	{
		origZ = 0;
	}
	for(int i = -1; i < 2; i++)
	{
		for(int j = -1; j < 2; j++)
		{
			if(i == 0 && j == 0){ continue;	}		//Skip the middle position

			float radius = sqrt(pow((x + (j * moveSpeed) - (float)origX), 2) + pow((z + (i * moveSpeed) - (float)origZ), 2));
			int index = 3 * (i + 1) + (j + 1) - (3 * (i + 1) + (j + 1)) / 5;		//translates to 0-7 index
			if(radius < 1 )
			{
				neighbourPos[index] = -100;
			} else
			{
				neighbourPos[index] += basePower / radius;
			}
		}
	}
}

void Enemy::clearPotential(const int staticBoard[455][256])
{
	//float radii[9];
	for(int i = -1; i < 2; i++)
	{
		for(int j = -1; j < 2; j++)
		{
			if(i == 0 && j == 0){ continue; }		//Skip the middle position

			int boardPosX = (int)x + j;
			int boardPosZ = (int)z + i;

			neighbourPos[3 * (i + 1) + (j + 1) - (3 * (i + 1) + (j + 1)) / 5] = staticBoard[boardPosX][boardPosZ];
		}
	}
}

void Enemy::clearPotential()
{
	for(int i = 0; i < 8; i++)
	{
		neighbourPos[i] = 0;
	}
}
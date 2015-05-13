#include "Enemy.h"

Enemy::Enemy()
{
	loadObj = new GObject();
	health = 20;
	type = MELEE;
	moveSpeed = 2.0f;
	this->x = 100.0f;
	this->z = 100.0f;
	Point colPoints [4] = {{x - 5.0, z - 5.0}, {x - 5.0, z + 5.0}, {x + 5.0, z - 5.0}, {x + 5.0, z + 5.0}};
	collisionRect = BoundingPolygon(colPoints, 4);

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
		health = 20 + 2 * waveNr;
		this->type = MELEE;
		moveSpeed = 1.0f;
		this->attackRange = MELEERANGE;
		break;
	case ANIMAL:
		loadObj = new GObject(objectFile, GL_TRIANGLES, texture);
		health = 30 + 3 * waveNr;
		this->type = ANIMAL;
		moveSpeed = 2.0f;
		this->attackRange = ANIMALRANGE;
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
		this->type = ANIMAL;
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

BoundingPolygon Enemy::getBounds() const
{
	return collisionRect;
}

bool Enemy::takeDamage(const int dmg)
{
	health -= dmg;
	return (health > 0);
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
	else
	{
		damage = 2;		//Animal damage
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
		int xMove = (highIndex + highIndex / 4) % 3 - 1;
		int zMove = (highIndex + highIndex / 4) / 3 - 1;
		this->x += xMove;
		this->z += zMove;
		loadObj->translate(xMove, 0.0, zMove);
		collisionRect.move(xMove, zMove);
	}
}

void Enemy::setPotential(int origX, int origZ, int basePower)
{
	//float radii[9];
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

void Enemy::clearPotential(const int staticBoard[256][256])
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
#include "Enemy.h"
#include "iostream"

Enemy::Enemy()
{
	loadObj = new GObject();
	health = 20;
	type = MELEE;
	moveSpeed = 2.0f;
	this->x = 100.0f;
	this->z = 100.0f;

	initPhero();
}

Enemy::Enemy(int type, float x, float z, GLuint texture)
{
	this->x = x;
	this->z = z;
	loadObj = nullptr;
	collisionRect = {x - 1, x + 1, z - 1, z + 1};
	switch ( type )
	{
	case MELEE:
		loadObj = new GObject( "enamie.obj", GL_TRIANGLES, texture );
		health = 20;
		this->type = MELEE;
		moveSpeed = 10.0f;	
		break;
	default:
		break;
	}
	loadObj->translate(x, 17, z);

	initPhero();
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
		this->type = TANK;
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

BoundingRect Enemy::getBounds() const
{
	return{x - 5, z - 5, x + 5, z + 5};
}

bool Enemy::takeDamage(const int dmg)
{
	health -= dmg;
	return (health > 0);
}

void Enemy::attack()
{
	if (type == MELEE)
	{

	}
	else if (type == RANGED)
	{

	}
	else
	{

	}
}

void Enemy::act(float playerX, float playerZ, int** board) //spelarens objekt eller plats
{
	bool attacking = false;
	int biggest = -100;
	float bestMoveX = 0.0f;
	float bestMoveZ = 0.0f;

	if (type == MELEE)		//atack if in range
	{
		if (abs(x - playerX) <= MELEERANGE && abs(z - playerZ) <= MELEERANGE)
		{
			attacking = true;
			attack();
		}
	}
	else if (type == RANGED)
	{
		if (abs(x - playerX) <= RANGEDRANGE && abs(z - playerZ) <= RANGEDRANGE)
		{
			attacking = true;
			attack();
		}
	}
	else if (type == TANK)
	{
		if (abs(x - playerX) <= TANKRANGE && abs(z - playerZ) <= TANKRANGE)
		{
			attacking = true;
			attack();
		}
	}

	if (attacking == false || board[(int)x][(int)z] >= 0)
	{
		float moveX = 0.0f;
		float moveZ = 0.0f;

		if (x < playerX)		//suggested movement
		{
			moveX++;
		}
		else if (x > playerX)
		{
			moveX--;
		}
		if (z < playerZ)
		{
			moveZ++;
		}
		else if (z > playerZ)
		{
			moveZ--;
		}

		if (board[(int)(x + moveX)][(int)(z + moveZ)] < 0 || board[(int)x][(int)z] < 0)
		{
			int testerX = (int)moveX;
			int testerZ = (int)moveZ;

			if (testerX == -1 && testerZ == -1)	//if trying to move down right
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{	//Tar ut platsen med högst värde och sparar undan den
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == -1 && testerZ == -1)
					{
						testerX++;	// (0, -1) down
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerX++; // (1, -1) down left
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2;
						testerZ++; // (-1, 0) right
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerZ++;		// (-1, 1) up right
					}
					else
					{
						testerX = 0;	//chosing path of least resistence
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == -1 && testerZ == 0)	//if trying to move right
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == -1 && testerZ == 0)
					{
						testerX += 2;
						testerZ--;	// (1,-1) up right
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2;	// (-1,-1) down right
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerX++;		//(0,-1) down
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerZ += 2;	// (0,1) up;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == 1 && testerZ == -1)	//if trying to move up right
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2;	// (-1,0)  right
						testerZ++;
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerZ++;	// (0,1) up
						testerX++;
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerX++;		//(1,1) up left
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerZ -= 2;	// (-1,-1) down right;
						testerX -= 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == 0 && testerZ == 1)	//if trying to move up
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 0 && testerZ == 1)
					{
						testerX--; // (-1,1) up right
					}
					else if (testerX == -1 && testerZ == 1)
					{
						testerX += 2; // (1,1) up left
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerZ--;	// (1,0) left
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerX -= 2;	// (-1,0) right
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == 1 && testerZ == 1)//if trying to move up left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 1 && testerZ == 1)
					{
						testerX--; // (0,1) up
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerX++;	// (1,0) left
						testerZ--;
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerX -= 2;	// (-1,1) up right
						testerZ++;
					}
					else if (testerX == -1 && testerZ == 1)
					{
						testerX += 2;	//(1, -1) down left
						testerZ -= 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == 1 && testerZ == 0)	//if trying to move left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 1 && testerZ == 0)
					{
						testerZ++; // (1,1) up left
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerZ -= 2; // (1,-1) down left
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX--; // (0,-1) down
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerZ += 2; // (0,1) up
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == 1 && testerZ == -1)	//if trying to move down left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 1 && testerZ == -1)
					{
						testerZ++;	// (1,0) left
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerX--;	// (0,-1) down
						testerZ--;
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerX++;		// (1,1) up left
						testerZ += 2;
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerX -= 2;	// (-1,-1) down right
						testerZ -= 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == 0 && testerZ == -1)	//if trying to move down
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < 0)
				{
					if (board[(int)(x + testerX)][(int)(z + testerZ)] > biggest)
					{
						biggest = board[(int)(x + testerX)][(int)(z + testerZ)];

						bestMoveX = testerX;
						bestMoveZ = testerZ;
					}

					if (testerX == 0 && testerZ == -1)
					{
						testerX++;	// (1,-1) down left
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2; // (-1,-1) down right
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerX += 2;	// (1,0) left
						testerZ++;
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerX -= 2; // (-1,0) right
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			
			moveX = bestMoveX * moveSpeed;	//Got where we are moving 
			moveZ = bestMoveZ * moveSpeed;
			setPheromone(x, z, board);
		}

		if (moveX != 0)
		{
			x = x + moveX;
			loadObj->translate(moveX, 0, 0);

		}
		if (moveZ != 0)
		{
			z = z + moveZ;
			loadObj->translate(0, 0, moveZ);
		}
	}
}

void Enemy::setPheromone(int x, int z, int** board)
{
	if (thirdLastX != 0 && thirdLastZ != 0)
	{
		board[thirdLastX][thirdLastZ] += 5000;
	}

	thirdLastX = secondLastX;
	thirdLastZ = secondLastZ;

	secondLastX = firstLastX;
	secondLastZ = firstLastZ;

	firstLastX = x;
	firstLastZ = z;

	board[firstLastX][firstLastZ] += -5000;
	board[secondLastX][secondLastZ] += -5000;
	board[thirdLastX][thirdLastZ] += -5000;
}

void Enemy::initPhero()
{
	firstLastX = 0;
	firstLastZ = 0;
	secondLastX = 0;
	secondLastZ = 0;
	thirdLastX = 0;
	thirdLastZ = 0;
}
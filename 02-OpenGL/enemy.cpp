#include "Enemy.h"

Enemy::Enemy()
{
	loadObj = new GObject();
	health = 20;
	type = MELEE;
	moveSpeed = 1.0f;
	this->x = 100.0f;
	this->z = 100.0f;
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
		moveSpeed = 1.0f;	
		break;
	default:
		break;
	}
	loadObj->translate(x, 17, z);
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
	if (type == MELEE)		//atack if in range
	{
		if (abs(x - playerX) <= 4 && abs(z - playerZ) <= 1)
		{
			attacking = true;
			attack();
		}
	}
	else if (type == RANGED)
	{
		if (abs(x - playerX) <= 16 && abs(z - playerZ) <= 8)
		{
			attacking = true;
			attack();
		}
	}
	else if (type == TANK)
	{
		if (abs(x - playerX) <= 6 && abs(z - playerZ) <= 2)
		{
			attacking = true;
			attack();
		}
	}
	if (attacking == false)	//If not attacking then move
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

		if(board[(int)(x + moveX)][(int)(z + moveZ)] < -1)
		{
			int testerX = (int)moveX;
			int testerZ = (int)moveZ;

			if (testerX == 1 && testerZ == 1)	//if trying to move down right
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == 1 && testerZ == 1)
					{
						testerX--;	// (0, 1) down
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerX--; // (-1, 1) down right
					}
					else if (testerX == -1 && testerZ == 1)
					{
						testerX += 2;
						testerZ--; // (1, 0) right
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerZ--;		// (1, -1) up right
					}
					else
					{
						testerX = 0;	//not moving
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == 1 && testerZ == 0)	//if trying to move right
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == 1 && testerZ == 0)
					{
						testerZ--;	// (1,-1) up right
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerZ += 2;	// (1,1) down right
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerX--;		//(0,1) down
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerZ -= 2;	// (0,-1) up;
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
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == 1 && testerZ == -1)
					{
						testerZ++;	// (1,0)  right
					}
					else if (testerX == 1 && testerZ == 0)
					{
						testerZ--;	// (0,-1) up
						testerX--;
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerX--;		//(-1,-1) up left
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerZ += 2;	// (1,1) down right;
						testerX += 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == 0 && testerZ == -1)	//if trying to move up
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == 0 && testerZ == -1)
					{
						testerX++; // (1,-1) up right
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2; // (-1,-1) up left
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerZ++;	// (-1,0) left
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerX += 2;	// (1,0) right
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}
			}
			else if (testerX == -1 && testerZ == -1)//if trying to move up left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == -1 && testerZ == -1)
					{
						testerX++; // (0,-1) up
					}
					else if (testerX == 0 && testerZ == -1)
					{
						testerX--;	// (-1,0) left
						testerZ++;
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerX += 2;	// (1,-1) up right
						testerZ--;
					}
					else if (testerX == 1 && testerZ == -1)
					{
						testerX -= 2;	//(-1, 1) down left
						testerZ += 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == -1 && testerZ == 0)	//if trying to move left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == -1 && testerZ == 0)
					{
						testerZ--; // (-1,-1) up left
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerZ += 2; // (-1,1) down left
					}
					else if (testerX == -1 && testerZ == 1)
					{
						testerX++; // (0,1) down
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerZ -= 2; // (0,-1) up
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == -1 && testerZ == 1)	//if trying to move down left
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == -1 && testerZ == 1)
					{
						testerZ--;	// (-1,0) left
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerX++;	// (0,1) down
						testerZ++;
					}
					else if (testerX == 0 && testerZ == 1)
					{
						testerX--;		// (-1,-1) up left
						testerZ -= 2;
					}
					else if (testerX == -1 && testerZ == -1)
					{
						testerX += 2;	// (1,1) down right
						testerZ += 2;
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			else if (testerX == 0 && testerZ == 1)	//if trying to move down
			{
				while (board[(int)(x + testerX)][(int)(z + testerZ)] < -1)
				{
					if (testerX == 0 && testerZ == 1)
					{
						testerX--;	// (-1,1) down left
					}
					else if (testerX == -1 && testerZ == 1)
					{
						testerX += 2; // (1,1) down right
					}
					else if (testerX == 1 && testerZ == 1)
					{
						testerX -= 2;	// (-1,0) left
						testerZ--;
					}
					else if (testerX == -1 && testerZ == 0)
					{
						testerX += 2; // (1,0) right
					}
					else
					{
						testerX = 0;
						testerZ = 0;
						break;
					}
				}

			}
			
			moveX = testerX * moveSpeed;	//Got where we are moving 
			moveZ = testerZ * moveSpeed;
		}
		
		if (moveX != 0)
		{
			board[(int)x][(int)z] = 0;		//Set our current pos as 0 in the board
			x = x + moveX;
			createNegativePotential(board, x, z, 2);	//tell the board where we are standing
			loadObj->translate(moveX, 0, 0);
		}
		if (moveZ != 0)
		{
			createPositivePotential(board, x, z, 3);
			board[(int)x][(int)z] = 0;
			z = z + moveZ;
			createNegativePotential(board, x, z, 3);

			loadObj->translate(0, 0, moveZ);
		}
	}
}

void Enemy::createNegativePotential(int** board, int posX, int posZ, int size)
{
	board[posZ][posX] += -4;

	int length = 0;
	for (int i = posZ - size; i <= posZ + size; i++)
	{
		for (int j = posX - size; j <= posX + size; j++)
		{
			length = sqrt(((j - posX)*(j - posX)) + ((i - posZ)*(i - posZ)));
			if (length < size)
			{
				if (length > 0)
					board[i][j] -= size - length;
			}
		}
	}
}

void Enemy::createPositivePotential(int** board, int posX, int posZ, int size)
{
	board[posZ][posX] += 0;

	int length = 0;
	for (int i = posZ - size; i <= posZ + size; i++)
	{
		for (int j = posX - size; j <= posX + size; j++)
		{
			length = sqrt(((j - posX)*(j - posX)) + ((i - posZ)*(i - posZ)));
			if (length < size)
			{
				if (length > 0)
					board[i][j] += size - length;
			}
		}
	}
}
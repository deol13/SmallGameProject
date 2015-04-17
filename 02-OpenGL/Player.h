#ifndef PLAYER_H
#define PLAYER_H

#include "GObject.h"

class Player 
{
private:
	int health;
	int gold;
	float x, z;
	int moveSpeed;
	GObject* loadObj;

public:
	enum Direction { UP, DOWN, LEFT, RIGHT };
	Player();
	Player(GLuint texture, float x, float y);
	void attack();
	void move( Direction dir);
	void takeDamage( int dmg);
	GObject* getGObject() const;
	//void update();

	float getX();
	float getZ();
};
#endif
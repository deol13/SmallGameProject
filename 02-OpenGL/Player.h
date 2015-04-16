#ifndef PLAYER_H
#define PLAYER_H

#include "GObject.h"

//enum Direction {UP, DOWN, LEFT, RIGHT};


class Player 
{
private:
	int health;
	int gold;
	int x, z;
	int moveSpeed;
	GObject* loadObj;
public:
	enum Direction { UP, DOWN, LEFT, RIGHT };
	Player();
	Player(GLuint texture, int x, int y);
	void attack();
	void move( Direction dir);
	void takeDamage( int dmg);
	GObject* getGObject() const;
	//void update();
};
#endif
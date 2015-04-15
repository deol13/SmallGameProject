#ifndef PLAYER_H
#define PLAYER_H

enum Direction {UP, DOWN, LEFT, RIGHT};

class Player 
{
private:
	int x, y;
public:
	Player();
	attack();
	move( Direction dir);
	takeDamage( int dmg);
};
#endif
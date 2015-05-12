#ifndef BOUNDINGRECT_H
#define BOUNDINGRECT_H

struct BoundingRect {
public:
	float minX, minZ, maxX, maxZ;

	bool collides(BoundingRect comp)
	{ 
		return (minX < comp.maxX && maxX > comp.minX && minZ < comp.maxZ && maxZ > comp.minZ); 
	}
	void move(float x, float z)
	{
		minX += x;
		maxX += x;
		minZ += z;
		maxZ += z;
	}
};
#endif
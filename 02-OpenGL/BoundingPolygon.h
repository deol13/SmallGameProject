#ifndef BOUNDINGPOlYGON_H
#define BOUNDINGPOlYGON_H
#include <vector>

struct Point
{
	float x, z;

	~Point(){}
	float dot(const Point& comp) const
	{
		return this->x * comp.x + this->z * comp.z;
	}
	Point operator+(const Point& comp)
	{
		return{this->x + comp.x, this->z + comp.z};
	}
	Point operator-(const Point& comp)
	{
		return{this->x - comp.x, this->z - comp.z};
	}
};

class BoundingPolygon
{
private:
	int nrOfEdges;
	std::vector<Point> points;
	std::vector<Point> normals;
	void findNormals();
public:
	BoundingPolygon();
	//BoundingPolygon(Point* points, int nrOfEdges);
	BoundingPolygon(Point points[], int nrOfEdges);
	~BoundingPolygon();
	bool collides(const BoundingPolygon& comp);
	void move(float x, float z);
	float findMin(Point axis) const;
	float findMax(Point axis) const;
};
//struct BoundingRect {
//public:
//	Vec2d centre;
//	Vec2d width;
//	Vec2d height;
//	
//
//	bool collides(BoundingRect comp)
//	{ 
//		Vec2d minPoint = comp.centre - comp.width - comp.height;
//		Vec2d maxPoint = comp.centre + comp.width + comp.height;
//		
//		
//		//return (minX < comp.maxX && maxX > comp.minX && minZ < comp.maxZ && maxZ > comp.minZ); 
//	}
//	void move(float x, float z)
//	{
//		centre.x += x;
//		centre.z += z;
//	}
//};
//
//struct BoundingTriangle {
//public:
//	Vec2d points[3];
//
//	bool collides(BoundingRect comp)
//	{
//		for(int i = 0; i < 3; i++)
//		{
//
//		}
//	//	return (minX < comp.maxX && maxX > comp.minX && minZ < comp.maxZ && maxZ > comp.minZ);
//	}
//	void move(float x, float z)
//	{
//		minX += x;
//		maxX += x;
//		minZ += z;
//		maxZ += z;
//	}
//};
#endif
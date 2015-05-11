#include "BoundingPolygon.h"

BoundingPolygon::BoundingPolygon()
{}
//BoundingPolygon::BoundingPolygon(Point* points, int nrOfEdges)
//{
//	for(int i = 0; i < nrOfEdges; i++)
//	{
//		this->points.push_back(points[i]);
//	}
//	this->nrOfEdges = nrOfEdges;
//	findNormals();
//}

BoundingPolygon::BoundingPolygon(Point points[], int nrOfEdges)
{
	for(int i = 0; i < nrOfEdges; i++)
	{
		this->points.push_back(points[i]);
	}
	this->nrOfEdges = nrOfEdges;
	findNormals();
}

BoundingPolygon::~BoundingPolygon(){}

bool BoundingPolygon::collides(const BoundingPolygon& comp)
{
	for(int i = 0; i < nrOfEdges; i++)
	{
		float min = findMin(normals[i]);
		float compMax = comp.findMax(normals[i]);
		if(min > compMax)
		{
			return false;
		}
		float max = findMax(normals[i]);
		float compMin = comp.findMin(normals[i]);
		if(max < compMin)
		{
			return false;
		}
	}
	for(int i = 0; i < comp.nrOfEdges; i++)
	{
		float min = findMin(comp.normals[i]);
		float compMax = comp.findMax(comp.normals[i]);
		if(min > compMax)
		{
			return false;
		}
		float max = findMax(comp.normals[i]);
		float compMin = comp.findMin(comp.normals[i]);
		if(max < compMin)
		{
			return false;
		}
	}
	return true;
}

void BoundingPolygon::move(float x, float z)
{
	for(int i = 0; i < nrOfEdges; i++)
	{
		points[i].x += x;
		points[i].z += z;
	}
}

float BoundingPolygon::findMin(Point axis) const
{
	float min = points[0].dot(axis);
	for(int i = 1; i < nrOfEdges; i++)
	{
		float temp = points[i].dot(axis);
		if(temp < min)
		{
			min = temp;
		}
	}
	return min;
}

float BoundingPolygon::findMax(Point axis) const
{
	float max = points[0].dot(axis);
	for(int i = 1; i < nrOfEdges; i++)
	{
		float temp = points[i].dot(axis);
		if(temp > max)
		{
			max = temp;
		}
	}
	return max;
}

void BoundingPolygon::findNormals()
{
	for(int i = 0; i < nrOfEdges - 1; i++)
	{
		normals.push_back({points[i].z - points[i + 1].z, points[i + 1].x - points[i].x});
	}
	normals.push_back({points[nrOfEdges - 1].z - points[0].z, points[0].x - points[nrOfEdges - 1].x});
}
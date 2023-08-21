#pragma once

#include <vector>
#include <map>
#include <array>



//class edge represents the edges of the model
class edge
{
public:
	int start;
	int end;
	int point_count;

	edge(int s, int e)
	{
		start = s;
		end = e;
		point_count = -1;
	};

	std::vector<int> faces;
};

//class Cartesian3 is for vertex 3D coordinates and normals
class Cartesian3
{
public:
	float x, y, z;
	Cartesian3(){};
	Cartesian3(float, float, float);
	void operator+=(Cartesian3);
	Cartesian3 operator+(Cartesian3);
	Cartesian3 operator-(Cartesian3);
	Cartesian3 operator*(float);
	Cartesian3& operator*=(float scalar);
	Cartesian3 operator/(float); 
	Cartesian3& operator/=(float); 
	Cartesian3 cross(Cartesian3& other);
	float magnitude(); 
	Cartesian3 mul(float a) const {
		return Cartesian3(a * x, a * y, a * z);
	}
	Cartesian3 add(const Cartesian3& other) const {
		return Cartesian3(x + other.x, y + other.y, z + other.z);
	}
	Cartesian3 minus(const Cartesian3& other) const {
		return Cartesian3(x - other.x, y - other.y, z - other.z);
	}
	std::vector<edge*>lines;
};



//class face contains vertex serial number of each face, not real 3D coordinates
class face
{
public:
	face(){};
	face(std::vector<int>);

	std::vector<int> points;
	std::vector<edge*> lines;
	int numberOfPoint;

};

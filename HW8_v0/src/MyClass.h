#pragma once

class MyPoint {
public:
	float x;
	float y;
	float z;
	bool end;
	MyPoint(float x,float y, float z = 0.0f, bool end = false) : x(x), y(y), z(z), end(end) { }
	MyPoint() { }
	bool operator == (MyPoint p) {
		return x == p.x && y == p.y && z == p.z;
	}
	bool isEndPoint() { return end;  }
};

class Point_3D {
public:
	float x, y, z;
	Point_3D() { }
	Point_3D(float x, float y, float z): x(x), y(y), z(z) { }
};

class Triangle_Index {
public:
	int p1, p2, p3;
	Triangle_Index(int p1, int p2, int p3): p1(p1), p2(p2), p3(p3) { }
};
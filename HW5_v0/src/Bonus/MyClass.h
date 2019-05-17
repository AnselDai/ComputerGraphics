#pragma once

class Point {
public:
	int x;
	int y;
	Point(int x, int y) : x(x), y(y) { }
	Point() { }
	bool operator == (Point p) {
		return x == p.x && y == p.y;
	}
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
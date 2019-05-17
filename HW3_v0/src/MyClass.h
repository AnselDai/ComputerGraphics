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
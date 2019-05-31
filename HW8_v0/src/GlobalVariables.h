#pragma once
#ifndef GLOBALVARIABLE
#define GLOBALVARIABLE

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include "MyClass.h"
#include <vector>

// 窗口大小
const int width = 800, height = 600;

// 贝塞尔曲线生成用到的变量
// 控制点
list<MyPoint> control_points;
float* control_points_arr = NULL;
// 曲线
vector<MyPoint> curve_points;
float* curve_points_arr = NULL;
// 中间点和线
vector<vector<MyPoint>> middle_points;
float* middle_points_arr = NULL;
MyPoint middle_point_on_curve[1];
bool canAddNewControlPoint = false;
MyPoint selectedPoint;
float step = 0.005;
float currentRatio = step;

unsigned int controlVAO = 0, controlVBO = 0;
unsigned int curveVAO = 0, curveVBO = 0;
unsigned int middleVAO = 0;
unsigned int middleVBO = 0;

#endif // !GLOBALVARAIBLE

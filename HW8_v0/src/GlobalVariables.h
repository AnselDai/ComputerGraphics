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

// ���ڴ�С
const int width = 800, height = 600;

// ���������������õ��ı���
// ���Ƶ�
list<MyPoint> control_points;
float* control_points_arr = NULL;
// ����
vector<MyPoint> curve_points;
float* curve_points_arr = NULL;
// �м�����
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

#pragma once

#include <iostream>
#include <GL/glew.h>

using namespace std;

struct Vec3
{
	float x;
	float y;
	float z;

	bool operator==(const Vec3& vec) {
		return this->x == vec.x && this->y == vec.y && this->z == vec.z;
	}
};

struct Vec2
{
	float x;
	float y;
};

struct Color
{
	float r;
	float g;
	float b;
	float a;
};

class Face
{
public:
	Face(Vec3 v1, Vec3 v2, Vec3 v3);
	~Face();

	void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void drawSelf();

public:
	Vec3 v1;
	Vec3 v2;
	Vec3 v3;
	Color color;
public:
	GLuint VAOId;
	GLuint textureId;
};


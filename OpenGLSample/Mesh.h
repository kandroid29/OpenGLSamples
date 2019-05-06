#pragma once

#include "Face.h"
#include <vector>
#include <GL/glew.h>

struct VertexOld {
	Vec3 pos;
	Color color;
	Vec2 texCoord;

	bool operator==(const VertexOld& vert) {
		return this->pos == vert.pos;
	}
};

class Mesh {
public:
	Mesh();
	~Mesh();

	void draw(GLuint program);
	void addFace(Face face);
public:
	int faceCount = 0;
	std::vector<Face> faces;
	GLbyte *data;
	int offset;

	std::vector<VertexOld> vertices;	//vertices without duplication

private:
	GLuint vao;
	GLuint vbo;
	GLuint textureId;
	//GLuint program;
};
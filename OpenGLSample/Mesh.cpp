#include "Mesh.h"
#include <soil/SOIL.h>
#include "LoadShaders.h"

#include "Utils.h"

void bufferSubData(int *offsetPtr, int size, const void *data) {
	glBufferSubData(GL_ARRAY_BUFFER, *offsetPtr, size, data);
	*offsetPtr += size;
}

void printBuffer(GLfloat *buffer, int size) {
	using namespace std;
	cout << "{";
	for (int i = 0; i < size; i++) {
		cout << buffer[i] << ", ";
	}
	cout << "}" << endl;
}

Mesh::Mesh() {
	int faceSize = sizeof(Face);
	data = (GLbyte *)malloc(sizeof(float) * (3 + 2) * 3 * 50);
}

Mesh::~Mesh() {
	free(data);
}

void Mesh::draw(GLuint program) {
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	glBindVertexArray(this->vao);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glDrawArrays(GL_TRIANGLES, 0, 3 * faces.size());
}

void Mesh::addFace(Face face)
{
	//memcpy(data + offset, &face, sizeof(float));
	faces.push_back(face);
	faceCount++;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int vertSize = sizeof(Vec3) + sizeof(Color) + sizeof(GLfloat) * 2;
	int faceSize = vertSize * 3;

	//glUseProgram(program);

	//bind buffers
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, faceSize * faces.size(), NULL, GL_STATIC_DRAW);

	GLfloat texCoords[8] = {
		0.f, 1.f,
		0.f, 0.f,
		1.f, 1.f,
		1.f, 0.f
	};

	int bufferSize = faceSize * faces.size();
	GLfloat *bufferData = (GLfloat *)malloc(bufferSize);

	int offset = 0;
	for (int i = 0; i < faces.size(); i++) {
		Face f = faces.at(i);

		/*Vertex v1 = { f.v1, f.color,{ texCoords[0], texCoords[1] } };
		Vertex v2 = { f.v2, f.color,{ texCoords[2], texCoords[3] } };
		Vertex v3 = { f.v3, f.color,{ texCoords[4], texCoords[5] } };

		if (std::find(vertices.begin(), vertices.end(), v1) == vertices.end()) {
			vertices.push_back(v1);
		}

		if (std::find(vertices.begin(), vertices.end(), v2) == vertices.end()) {
			vertices.push_back(v2);
		}

		if (std::find(vertices.begin(), vertices.end(), v3) == vertices.end()) {
			vertices.push_back(v3);
		}*/


		bufferSubData(&offset, sizeof(Vec3), &f.v1);
		bufferSubData(&offset, sizeof(Color), &f.color);
		bufferSubData(&offset, sizeof(GLfloat) * 2, texCoords);
		//glGetBufferSubData(GL_ARRAY_BUFFER, 0, faceSize, bufferData);

		bufferSubData(&offset, sizeof(Vec3), &f.v2);
		bufferSubData(&offset, sizeof(Color), &f.color);
		bufferSubData(&offset, sizeof(GLfloat) * 2, &texCoords[2]);

		bufferSubData(&offset, sizeof(Vec3), &f.v3);
		bufferSubData(&offset, sizeof(Color), &f.color);
		bufferSubData(&offset, sizeof(GLfloat) * 2, &texCoords[4]);
	}
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, bufferData);
	printBuffer(bufferData, bufferSize / sizeof(GLfloat));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertSize, (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertSize, (GLvoid *)sizeof(Vec3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertSize, (GLvoid *)(sizeof(Vec3) + sizeof(Color)));
	glEnableVertexAttribArray(2);



	//bind texture
	glGenTextures(1, &this->textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	int imageWidth, imageHeight;
	GLubyte *imagePixels = SOIL_load_image("E:\\Pictures\\dota2_001.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	int pixelType = GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, pixelType, imagePixels);

	/*glUniform1i(glGetUniformLocation(program, "tex"), 0);*/

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


}

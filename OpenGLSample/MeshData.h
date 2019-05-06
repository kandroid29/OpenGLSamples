#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "Mesh.h"

using namespace std;
using namespace glm;

struct Position
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	bool operator==(const Position& pos) {
		return this->x = pos.x && this->y == pos.y && this->z == pos.z;
	}
};

struct TextureCoordination
{
	GLfloat x;
	GLfloat y;

	bool operator==(const TextureCoordination& texCoord) {
		return this->x == texCoord.x && this->y == texCoord.y;
	}
};

// 表示obj文件中一个顶点的位置、纹理坐标和法向量 索引
struct VertexCombineIndex
{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

struct Vertex {
	Position position;
	TextureCoordination texCoord;

	bool operator==(const Vertex& vert) {
		return this->position == vert.position && this->texCoord == vert.texCoord;
	}
};

struct MeshData {
	vector<Position> positions;
	vector<TextureCoordination> texCoords;

	vector<GLuint> indices;
	vector<Vertex> vertices;	//vertices without duplication

	GLuint vertArrayId;
	GLuint elemBufferId;

	//API 2
	bool loadObjFile(const char *path) {
		ifstream file(path);
		string line;

		this->vertices.clear();
		this->indices.clear();

		while (getline(file, line)) {
			if (line.substr(0, 2) == "vt") {
				string values = line.substr(2);
				istringstream s(values);
				TextureCoordination texCoord;
				s >> texCoord.x;
				s >> texCoord.y;
				texCoord.y = 1 - texCoord.y;  // 注意这里加载的dds纹理 要对y进行反转
				this->texCoords.push_back(texCoord);
			}
			else if (line.substr(0, 2) == "vn") {
				//do nothing
			}
			else if (line.substr(0, 1) == "v") {
				string values = line.substr(1);
				istringstream s(values);
				Position pos;
				s >> pos.x;
				s >> pos.y;
				s >> pos.z;
				this->positions.push_back(pos);
			}
			else if (line.substr(0, 1) == "f") {
				istringstream vtns(line.substr(1));
				string vtn;

				vector<VertexCombineIndex> combineIndices;
				Vertex faceVertices[4];
				int vertCount = 0;
				while (vtns >> vtn) {
					VertexCombineIndex vertComIndex;

					replace(vtn.begin(), vtn.end(), '/', ' ');
					istringstream ivtn(vtn);
					if (vtn.find("  ") != string::npos) // 没有纹理数据
					{
						cerr << "Error:ObjLoader, no texture data found within file:"
							<< path << endl;
						return false;
					}
					ivtn >> vertComIndex.posIndex >> vertComIndex.textCoordIndex >> vertComIndex.normIndex;
					vertComIndex.posIndex--;
					vertComIndex.textCoordIndex--;
					vertComIndex.normIndex--;

					faceVertices[vertCount].position = positions[vertComIndex.posIndex];
					faceVertices[vertCount].texCoord = texCoords[vertComIndex.textCoordIndex];

					//this->vertices.push_back(vert);

					combineIndices.push_back(vertComIndex);

					vertCount++;
				}

				GLuint vertIndices[6] = {
					0, 1, 2,
					0, 2, 3
				};

				if (vertCount >= 3) {
					for (int i = 0; i < 3; i++) {
						cout << "[addVertexIfNotExists: before]" << endl;
						printData();

						GLuint vertIndex = addVertexIfNotExists(faceVertices[vertIndices[i]]);

						cout << "[addVertexIfNotExists: after1]" << endl;
						printData();
						cout << endl;

						this->indices.push_back(vertIndex);
					}

					if (vertCount == 4) {
						for (int i = 3; i < 6; i++) {
							GLuint vertIndex = addVertexIfNotExists(faceVertices[vertIndices[i]]);
							this->indices.push_back(vertIndex);
						}
					}
				}
			}
			else {
				//do nothing
			}
		}

		cout << "print in loadObjFile()" << endl;
		printData();

		lockData2();
		return true;
	}

	int addVertexIfNotExists(Vertex vertex) {
		//implementation 2
		int index = -1;
		for (int i = 0; i < this->vertices.size(); i++) {
			Vertex verti = vertices[i];
			if (verti == vertex) {
				index = i;
				break;
			}
		}

		if (index >= 0) {
			return index;
		}
		else {
			int vertIndex = vertices.size();
			vertices.push_back(vertex);
			return vertIndex;
		}


		//implementation 1
		/*vector<Vertex>::iterator it = find(vertices.begin(), vertices.end(), vertex);
		if (it == vertices.end()) {
			int vertIndex = vertices.size();

			this->vertices.push_back(vertex);

			return vertIndex;
		}
		else {
			return std::distance(vertices.begin(), it);
		}*/
	}

	void lockData2() {
		glGenVertexArrays(1, &this->vertArrayId);
		glBindVertexArray(vertArrayId);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

		int positionLoc = 0;
		int texCoordLoc = 1;

		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)0);
		glEnableVertexAttribArray(positionLoc);

		glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) sizeof(Position));
		glEnableVertexAttribArray(texCoordLoc);

		glGenBuffers(1, &this->elemBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elemBufferId);
		printIndices();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	}

	void printData() {
		for (int i = 0; i < this->vertices.size(); i++) {
			Vertex vert = this->vertices[i];
			cout << vert.position.x << ", " << vert.position.y << ", " << vert.position.z << ", "
				<< vert.texCoord.x << ", " << vert.texCoord.y << ";" << endl;
		}
	}

	void printIndices() {
		cout << "[Indices]" << endl;
		for (int i = 0; i < indices.size(); i++) {
			GLuint index = indices[i];
			cout << index << ", ";
		}
	}

	//API 1

	void setIndices(GLuint *data, size_t count) {
		indices.clear();
		for (int i = 0; i < count; i++) {
			indices.push_back(data[i]);
		}
	}

	void addPositions(Position *positions, size_t size) {
		for (int i = 0; i < size; i++) {
			Position pos = positions[i];
			this->positions.push_back(pos);
		}
	}

	void addTextureCoords(TextureCoordination *texCoords, size_t size) {
		this->texCoords.clear();
		for (int i = 0; i < size; i++) {
			TextureCoordination coord = texCoords[i];
			this->texCoords.push_back(coord);
		}
	}

	void lockData() {
		GLuint arrayBuffers[2];

		glGenVertexArrays(1, &vertArrayId);
		glBindVertexArray(vertArrayId);

		glGenBuffers(2, arrayBuffers);

		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Position) * this->positions.size(), &this->positions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TextureCoordination) * this->texCoords.size(), &this->texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		glEnableVertexAttribArray(1);


		glGenBuffers(1, &this->elemBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elemBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
	}

	void drawSelf(GLuint textureId) {
		//start drawing
		glBindVertexArray(this->vertArrayId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elemBufferId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, (const void *)0);


	}
};
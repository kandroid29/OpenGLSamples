#include "Face.h"
#include <GL/glew.h>
#include <soil/SOIL.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))



Face::Face(Vec3 v1, Vec3 v2, Vec3 v3)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;

	color = { 0.9f, 0.1f, 0.6f, 1.0f };

	Color colors[3] = {
		color,
		color,
		color,
	};

	GLuint Buffers[3];
	
	GLfloat texCoords[6] = {
		0.f, 1.f - 0.f,
		0.f, 1.f - 1.f,
		1.f, 1.f - 0.f
	};

	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(this->VAOId);

	//绑定位置数组
	glGenBuffers(3, Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3, (void *)this, GL_STATIC_DRAW);

	int attrPosition = 0;
	glVertexAttribPointer(attrPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrPosition);

	//绑定颜色数组
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, (void *)colors, GL_STATIC_DRAW);

	int attrColor = 1;
	glVertexAttribPointer(attrColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrColor);

	//綁定纹理坐标
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 3, (void *)texCoords, GL_STATIC_DRAW);

	int attrTexCoord = 2;
	glVertexAttribPointer(attrTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrTexCoord);

	ShaderInfo shaders[] = {
		{GL_VERTEX_SHADER, "triangles.vert"},
		{GL_FRAGMENT_SHADER, "triangles.frag"},
		{GL_NONE, NULL}
	};

	GLuint program = loadShaders(shaders);
	glUseProgram(program);

	//setup texture
	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureId);


	int imageWidth, imageHeight;
	GLenum pixelType;


	/*GLfloat localPixels[] = {
		1.f, 0.f, 0.f,	1.f, 1.f,1.f,	1.f, 0.f, 0.f,
		1.f, 1.f, 1.f,	0.f, 0.f,1.f,	1.f, 1.f, 1.f,
		0.f, 1.f, 0.f,	1.f, 1.f, 1.f,	1.f, 0.f, 0.f
	};
	imageWidth = 3, imageHeight = 3, pixelType = GL_FLOAT;*/

	GLubyte *imagePixels = SOIL_load_image("E:\\Pictures\\dota2_001.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	pixelType = GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, pixelType, imagePixels);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3, 3, 0, GL_RGB, GL_FLOAT, imagePixels);

	glUniform1i(glGetUniformLocation(program, "tex"), 0);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


}


Face::~Face()
{
}

void Face::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	color = { r, g, b, a };

	Color colors[3] = {
		color,
		color,
		color,
	};

	glBindVertexArray(VAOId);

	//绑定颜色数组
	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, (void *)colors, GL_STATIC_DRAW);

	int attrColor = 1;
	glVertexAttribPointer(attrColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrColor);

}

void Face::drawSelf() {
	cout << "drawing face" << endl;

	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glBindVertexArray(this->VAOId);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

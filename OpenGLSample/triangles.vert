#version 430 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 texCoord;
//layout( location = 2 ) in vec4 vColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//out vec4 oColor;
out vec2 vTexCoord;

void main()
{
    gl_Position = projection * view * model * vPosition;
	//oColor = vColor;
	vTexCoord = texCoord;
}

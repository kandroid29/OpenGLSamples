#version 430 core

//in vec4 oColor;
in vec2 vTexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
	color = texture(tex, vTexCoord);
	//color = oColor;
}

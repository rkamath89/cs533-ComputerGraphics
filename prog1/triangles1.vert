#version 430 core

layout(location = 1) in vec4 vPosition1;
layout(location = 3) in vec4 vertexColor;

out vec4 colour;
void
main()
{
	colour = vertexColor;
    gl_Position = vPosition1;
}

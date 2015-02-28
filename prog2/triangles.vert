#version 430 core

layout(location = 0) in vec4 vPosition;
layout(location = 3) in vec4 vertexColor;
uniform mat4 mvp;


out vec4 colour;
void
main()
{
	colour = vertexColor;
    gl_Position = mvp * vPosition;
}

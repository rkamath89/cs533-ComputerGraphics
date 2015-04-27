#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 texIndex;

uniform mat4 MVPMatrix;	// full model-view-projection matrix

out vec3 Color;
out vec2 tIndex;

void
main()
{
    gl_Position = MVPMatrix * vertexPosition;
	tIndex = texIndex;
    Color = vertexColor;
}
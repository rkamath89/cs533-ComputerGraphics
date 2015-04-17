#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 7) in vec2 texIndex;
layout(location = 4) in vec3 normal;

uniform mat4 MVPMatrix;	// full model-view-projection matrix
uniform mat4 MVMatrix;	// model and view matrix
uniform mat4 NormalMatrix;	// transformation matrix for normals to eye coordinates

out vec2 tIndex;
out vec3 Normal;	// vertex normal in eye coordinates
out vec4 Position;	// vertex position in eye coordinates
void
main()
{
	Normal = normalize(mat3(MVPMatrix) * normal);
	Position = MVPMatrix * vec4(vPosition, 1.0f);
    gl_Position = MVPMatrix * vec4(vPosition,1.0f);
	tIndex = texIndex;
}

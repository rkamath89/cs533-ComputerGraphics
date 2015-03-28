#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 kA;
layout(location = 3) in vec3 kD;
layout(location = 4) in vec3 normal;
uniform mat4 mvp;
vec3 lightSource = normalize(vec3(11.0, 11.0, 11.0));

out vec4 colour;
void
main()
{
	vec3 normalizedNormal = normalize(normal);
	vec3 newColor = kA + (max(dot(lightSource,normalizedNormal),0) * kD);
	colour = vec4(newColor,1.0f);
    gl_Position = mvp * vec4(vPosition,1.0f);
}

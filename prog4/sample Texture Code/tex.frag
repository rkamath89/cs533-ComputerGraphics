#version 430 core

in vec3 Color;
in vec2 tIndex;	// texture map indices
out vec4 fColor;

uniform sampler2D tex;

void
main()
{
	vec4 outColor = texture(tex, tIndex);
	if (outColor.a == 0)
		discard;
	else
		fColor = outColor;
}
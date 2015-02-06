#version 430 core

layout(location = 2) in vec4 vPosition2;

void
main()
{
    gl_Position = vPosition2;
}

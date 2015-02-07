#version 430 core


uniform vec4 customColour ={0.0f, 0.0f, 1.0f, 0.0f};
out vec4 fColor;

void
main()
{
    fColor = customColour;
}

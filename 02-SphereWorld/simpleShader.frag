#version 460

layout (location = 0) out vec4 color;

in vec4 fColor;
in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fPosition;

void main(void)
{
    color = fColor;
}
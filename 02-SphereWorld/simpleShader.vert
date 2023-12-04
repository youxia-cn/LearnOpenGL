#version 460

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 1) in vec2 vTexCoord;

out vec4 fColor;
out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fPosition;

void main(void)
{
    mat4 MV_matrix = view_matrix * model_matrix;
    gl_Position = projection_matrix * view_matrix * model_matrix * vPosition;
    fPosition =  MV_matrix * vPosition;
    fNormal = normalize(transpose(inverse(mat3(MV_matrix))) * vNormal);
    fTexCoord = vTexCoord;
    fColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
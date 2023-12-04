#version 460

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fPosition;
out vec4 fWorldPosition;

void main(void)
{
    mat4 MV_matrix = view_matrix * model_matrix;
    gl_Position = projection_matrix * view_matrix * model_matrix * vPosition;
    fPosition =  MV_matrix * vPosition;
    fNormal = normalize(transpose(inverse(mat3(model_matrix))) * vNormal);
    fWorldPosition = model_matrix * vPosition;
    fTexCoord = vTexCoord;
}
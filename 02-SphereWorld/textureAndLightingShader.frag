#version 460

layout (location = 0) out vec4 color;

in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fPosition; //摄像机坐标
in vec4 fWorldPosition; //世界坐标

uniform vec3 sun_position;
uniform vec3 eye_position;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float ns;
uniform int hasMapKd;
uniform sampler2D mapKd;
uniform int hasMapKs;
uniform sampler2D mapKs;

void main(void)
{
    vec4 lightColor = vec4(1.0f);
    vec4 lightAmbient = vec4(0.2f);
    vec3 sun_direction = normalize(sun_position - vec3(0.0f));
    vec3 eye_direction = normalize(eye_position - vec3(fWorldPosition));
    float diffuse = max(0.0f, dot(fNormal, sun_direction));
    vec3 halfDirection = (sun_direction + eye_direction)/2;
    float specular = max(0.0f, dot(fNormal, halfDirection));
    
    vec4 ambient_color;
    if(hasMapKd!=0){
        ambient_color = texture(mapKd, fTexCoord) * lightAmbient;
    }else{
        ambient_color = vec4(ka, 1.0f) * lightAmbient;
    }
    vec4 diffuse_color;
    if(hasMapKd!=0){
        diffuse_color = texture(mapKd, fTexCoord) * diffuse * lightColor;
    }else{
        diffuse_color = vec4(kd,1.0f) * diffuse * lightColor;
    }

    vec4 specular_color;
    if(hasMapKs!=0){
        specular_color = texture(mapKs, fTexCoord) * pow(specular, ns) * lightColor;
    }else{
        specular_color = vec4(ks,1.0f) * pow(specular, ns) * lightColor;
    }
    color = ambient_color + diffuse_color + specular_color;
}
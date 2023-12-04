#version 460

layout (location = 0) out vec4 color;

in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fPosition; //摄像机坐标
in vec4 fWorldPosition; //世界坐标

uniform vec3 positionOfLight1;
uniform vec3 positionOfLight2;
uniform vec3 cameraPosition;

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
    vec3 directionOfLight1 = normalize(positionOfLight1 - vec3(0.0f)); //光源都按平行光处理
    vec3 directionOfLight2 = normalize(positionOfLight2 - vec3(0.0f));
    vec3 cameraDirection = normalize(cameraPosition - vec3(fWorldPosition));
    float diffuse1 = max(0.0f, dot(fNormal, directionOfLight1));
    float diffuse2 = max(0.0f, dot(fNormal, directionOfLight2));
    vec3 halfDirection1 = (directionOfLight1 + cameraDirection)/2;
    vec3 halfDirection2 = (directionOfLight2 + cameraDirection)/2;
    float specular1 = max(0.0f, dot(fNormal, halfDirection1));
    float specular2 = max(0.0f, dot(fNormal, halfDirection1));
    
    vec4 ambient_color;
    if(hasMapKd!=0){
        ambient_color = texture(mapKd, fTexCoord) * lightAmbient;
    }else{
        ambient_color = vec4(ka, 1.0f) * lightAmbient;
    }
    vec4 diffuse_color1;
    if(hasMapKd!=0){
        diffuse_color1 = texture(mapKd, fTexCoord) * diffuse1 * lightColor;
    }else{
        diffuse_color1 = vec4(kd,1.0f) * diffuse1 * lightColor;
    }
    vec4 diffuse_color2;
    if(hasMapKd!=0){
        diffuse_color2 = texture(mapKd, fTexCoord) * diffuse2 * lightColor;
    }else{
        diffuse_color2 = vec4(kd,1.0f) * diffuse2 * lightColor;
    }
    vec4 specular_color1;
    if(hasMapKs!=0){
        specular_color1 = texture(mapKs, fTexCoord) * pow(specular1, ns) * lightColor;
    }else{
        specular_color1 = vec4(ks,1.0f) * pow(specular1, ns) * lightColor;
    }
    vec4 specular_color2;
    if(hasMapKs!=0){
        specular_color2 = texture(mapKs, fTexCoord) * pow(specular2, ns) * lightColor;
    }else{
        specular_color2 = vec4(ks,1.0f) * pow(specular2, ns) * lightColor;
    }
    color = ambient_color + diffuse_color1 + specular_color1 + diffuse_color2 + specular_color2;
}
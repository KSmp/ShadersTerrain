#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPos;

out vec2 uv;
out vec3 posVS;
out vec3 toCameraVector;

const float tiling = 6.0;

void main()
{
    uv = aTexCoords * tiling;  
    posVS = aPos;

    vec4 worldPos = model * vec4(posVS, 1.0);
    toCameraVector = viewPos - worldPos.xyz;
    gl_Position = projection * view * worldPos;
}
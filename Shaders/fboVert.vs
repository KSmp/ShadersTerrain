#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 uv;
out vec3 posVS;


void main()
{
    uv = aTexCoords;  
    posVS = aPos;
    gl_Position = vec4(posVS, 1.0);
}
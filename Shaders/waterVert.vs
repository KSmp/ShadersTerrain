#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoordsVS;
out vec3 posVS;


void main()
{
    texCoordsVS = aTexCoords;  
    posVS = aPos;
    gl_Position = projection * view * vec4(posVS, 1.0);
}
#version 410 core

out vec4 FragColor;

uniform sampler2D screen;

in vec2 uv;

void main()
{   
    FragColor = texture(screen, uv);
}
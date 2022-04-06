#version 410 core

uniform sampler2D reflection;
uniform sampler2D refraction;

in vec2 uv;

out vec4 FragColor;

void main()
{   
    vec4 reflect = texture(reflection, uv);
    vec4 refract = texture(refraction, uv);
    FragColor = mix(reflect, refract, 0.5);
}
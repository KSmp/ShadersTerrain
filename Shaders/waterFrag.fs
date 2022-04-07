#version 410 core

uniform sampler2D reflection;
uniform sampler2D refraction;

uniform int screenW;
uniform int screenH;

in vec2 uv;

out vec4 FragColor;

void main()
{   
    vec2 ndcXY = gl_FragCoord.xy;
    vec2 reflectTex = vec2((ndcXY.x) / screenW, (-ndcXY.y) / screenH);
    vec2 refractTex = vec2((ndcXY.x) / screenW, (ndcXY.y) / screenH);


    vec4 reflect = texture(reflection, reflectTex);
    vec4 refract = texture(refraction, refractTex);
    FragColor = mix(reflect, refract, 0.4);
}
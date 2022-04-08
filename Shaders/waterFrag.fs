#version 410 core

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudv;
uniform sampler2D waterNormals;
uniform sampler2D depthMap;

uniform int screenW;
uniform int screenH;

uniform float time;
uniform float near;
uniform float far;

in vec2 uv;
in vec3 toCameraVector;

out vec4 FragColor;

const float distortionScale = 0.01;

vec3 lightDir = vec3(0.0, -1.0, 0.0);
vec3 lightCol = vec3(1.0);
float specularStrength = 0.6;
float shine = 20.0;

vec3 getSpecular(vec3 norm);

void main()
{   
    float waveSpeed = time * 0.1;

    // Get texture coords
    vec2 ndcXY = gl_FragCoord.xy;
    vec2 reflectTex = vec2((ndcXY.x) / screenW, (-ndcXY.y) / screenH);
    vec2 refractTex = vec2((ndcXY.x) / screenW, (ndcXY.y) / screenH);


    // Depth
    float depth = texture(depthMap, refractTex).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    float waterDepth = floorDistance - waterDistance;


    // Distortion
    vec2 d1 = texture(dudv, vec2(uv.x + waveSpeed, uv.y)).rg * 2.0 - 1.0;
    vec2 d2 = texture(dudv, vec2(-uv.x + waveSpeed, uv.y)).rg * 2.0 - 1.0;
    vec2 d3 = texture(dudv, vec2(-uv.x, -uv.y + waveSpeed)).rg * 2.0 - 1.0;

    vec2 distortion = (d1 + d2 + d3) * distortionScale * clamp((waterDepth / 20.0), 0.0, 1.0);

    reflectTex += distortion;
    reflectTex.x = clamp(reflectTex.x, 0.001, 0.999);
    reflectTex.y = clamp(reflectTex.y, -0.999, -0.001);

    refractTex += distortion;
    refractTex = clamp(refractTex, 0.001, 0.999);


    // Get reflection and refraction
    vec4 reflectionTexture = texture(reflection, reflectTex);
    vec4 refractionTexture = texture(refraction, refractTex);

    // Normal
    vec4 normalMap = texture(waterNormals, distortion);
    vec3 normal = vec3(normalMap.r * 2.0 - 1.0, normalMap.b * 2.0, normalMap.g * 2.0 - 1.0);
    normal = normalize(normal);

    // Fresnel effect
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, normal);

    // Specular
    vec3 reflectedLight = reflect(normalize(lightDir), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shine);
    vec3 highlights = lightCol * specular * specularStrength  * clamp((waterDepth / 5.0), 0.0, 1.0);

    // Output
    FragColor = mix(reflectionTexture, refractionTexture, refractiveFactor);
    FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(highlights, 0.0);
    FragColor.a = clamp((waterDepth / 5.0), 0.0, 1.0);
}
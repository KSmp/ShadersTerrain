#version 410 core

in vec3 normES;
in vec3 posES;
in vec2 texCoordsES;
in float visibility;

out vec4 FragColor;

vec3 lightDir = vec3(0.0, -1.0, 0.0);
vec3 lightCol = vec3(1.0);
vec3 objCol = vec3(0.5, 0.5, 0.5);

float ambientFactor = 0.7;
float specularStrength = 0.4;
float shine = 128;

uniform sampler2D rockTexture;
uniform sampler2D mossTexture;
uniform sampler2D snowTexture;

uniform float scale;
uniform vec4 skyColour;
uniform float waterLevel;

void getObjCol();
vec3 getDirectionalLight(vec3 norm);
vec4 triPlanar();

void main()
{   
    getObjCol();
    FragColor = vec4(getDirectionalLight(normES), 1.0);
    FragColor = mix(skyColour, FragColor, visibility);
}

vec4 triPlanar(sampler2D textureToSample, float scale = 1.0) {
    vec3 blendPercent = normalize(abs(normES));
    float blendSum = (blendPercent.x + blendPercent.y + blendPercent.z);
    blendPercent = blendPercent/vec3(blendSum);

    vec4 xaxis = texture(textureToSample, posES.yz * scale);
    vec4 yaxis = texture(textureToSample, posES.xz * scale);
    vec4 zaxis = texture(textureToSample, posES.xy * scale);

    return xaxis * blendPercent.x + yaxis * blendPercent.y + zaxis * blendPercent.z;
}

void getObjCol() {
    float height = posES.y/scale;
    vec3 green = vec3(0.3, 0.35, 0.15);
    vec3 gray = vec3(0.5, 0.4, 0.5);
    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 rock = triPlanar(rockTexture, 0.05).rgb;
    vec3 moss = triPlanar(mossTexture, 0.1).rgb;

    objCol = green;

    if (height > 0.9) {
        objCol = white;
    } else if (height > 0.6) {
        objCol = mix(gray, white, smoothstep(0.7, 0.9, height));
    } else if (height > 0.3) {
        objCol = mix(green, gray, smoothstep(0.3, 0.4, height));
    }

    if (posES.y < waterLevel) {
        objCol = vec3(0.761, 0.698, 0.52);
    } 
}

vec3 getDirectionalLight(vec3 norm) {
    vec3 directionalLight = vec3(0.0);

    // ambient
    vec3 ambientColor = lightCol * objCol * ambientFactor;

    // diffuse
    float diffuseFactor = dot(norm, -lightDir);
    diffuseFactor = max(diffuseFactor, 0.0);

    vec3 diffuseColor = lightCol * objCol * diffuseFactor;

    // specular
    // vec3 viewDir = normalize(viewPos - posWS);

    // Blinn Phong
    // vec3 halfway = normalize(lightDir + viewDir);
    // float specularFactor = dot(halfway, norm);
    // specularFactor = max(specularFactor, 0.0);
    // specularFactor = pow(specularFactor, shine);

    vec3 specularColor = vec3(0.0);

    // specularColor = lightCol * specularFactor * specularStrength;

    directionalLight = ambientColor + diffuseColor + specularColor;

    return directionalLight;
}
#version 410 core

in vec3 normES;
in vec3 posES;

out vec4 FragColor;

vec3 lightDir = vec3(0.0, -1.0, 0.0);
vec3 lightCol = vec3(1.0);
vec3 objCol = vec3(0.5, 0.5, 0.5);

float ambientFactor = 0.7;
float specularStrength = 0.4;
float shine = 128;

void getObjCol();
vec3 getDirectionalLight(vec3 norm);

void main()
{   
    getObjCol();
    FragColor = vec4(getDirectionalLight(normES), 1.0);
}

void getObjCol() {
    float scale = 10.0;
    float height = posES.y/scale;
    vec3 green = vec3(0.3, 0.35, 0.15);
    vec3 gray = vec3(0.5, 0.4, 0.5);

    if (height > 0.4) {
        objCol = mix(green, gray, smoothstep(0.3, 1.0, height));
    } else {
        objCol = gray;
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
    // vec3 halfway = normalize(-lightDir + viewDir);
    // float specularFactor = dot(halfway, norm);
    // specularFactor = max(specularFactor, 0.0);
    // specularFactor = pow(specularFactor, shine);

    vec3 specularColor = vec3(0.0);

    // specularColor = lightCol * specularFactor * specularStrength;

    directionalLight = ambientColor + diffuseColor + specularColor;

    return directionalLight;
}
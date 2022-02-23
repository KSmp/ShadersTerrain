#version 410 core

in vec3 normES;

out vec4 FragColor;

vec3 lightDir = vec3(0.0, -1.0, 0.0);
vec3 lightCol = vec3(1.0);
vec3 objCol = vec3(0.5, 0.5, 0.5);

float ambientFactor = 0.7;
float specularStrength = 0.4;
float shine = 128;

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

void main()
{    
    FragColor = vec4(getDirectionalLight(normES), 1.0);
    FragColor = vec4(normES, 1.0);
}

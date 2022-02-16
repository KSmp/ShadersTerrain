#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;

out vec3 posVS; 
out vec3 normVS;

void main()
{
        normVS = aNormals ; 
		posVS = aPos;
}


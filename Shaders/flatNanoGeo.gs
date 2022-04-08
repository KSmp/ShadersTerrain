#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform vec4 plane;
in vec3 posES[];
in vec2 texCoordsES[];
in float visibility[];

out vec3 normG;
out vec3 posG;
out vec2 texCoordsG;
out float visibilityG;

vec3 getSurfaceNormal() {
	vec3 posA = gl_in[0].gl_Position.xyz;
	vec3 posB = gl_in[1].gl_Position.xyz;
	vec3 posC = gl_in[2].gl_Position.xyz;

	vec3 edgeA = posA - posB;
	vec3 edgeB = posC - posB;

	return normalize(cross(edgeA, edgeB));
}


void main() {
	vec4 pos = vec4(0.0);

	for (int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		normG = getSurfaceNormal();
		posG = posES[i];
		texCoordsG = texCoordsES[i];
		visibilityG = visibility[i];
		gl_ClipDistance[0] = dot(vec4(posES[i], 1.0), plane);
		EmitVertex();
		pos = pos + gl_in[i].gl_Position;
	}

	EndPrimitive();
}
#version 330 core
layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;
vec3 getNormal() ;

in vec3 posES[] ;
in vec3 normES[] ;

out vec3 normG;
out vec3 posG;


void main()
{
   for(int i = 0 ; i < 3; i++)
   {
      gl_Position = gl_in[i].gl_Position ;
      posG = posES[i] ;
      normG = getNormal();
      //gNormals = normES[i];
      EmitVertex() ;
  }
     EndPrimitive() ;
}


vec3 getNormal()
{
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(a, b));
}
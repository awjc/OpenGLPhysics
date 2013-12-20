#version 150

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec4 lightVector;

uniform float shrink;
uniform float explode;


in vec3 gN[3];
in vec3 gL[3];
in vec3 gV[3];

out vec3 fN;
out vec3 fL;
out vec3 fV;

void main() {

  // convert incoming values to world coordinates to get scale right
  vec4 p[3];
  p[0] = model * gl_in[0].gl_Position;
  p[1] = model * gl_in[1].gl_Position;
  p[2] = model * gl_in[2].gl_Position;
  
  vec4 cent = vec4(p[0] + p[1] + p[2]) / 3;
  
  vec3 a = p[0].xyz;
  vec3 b = p[1].xyz;
  vec3 c = p[2].xyz;
  vec3 normal = normalize(cross(b - a, c - a));
  
  for (int i = 0; i < 3; ++i)
  {
    
    //vec4 v = cent + (gl_in[i].gl_Position - cent) * shrink;
    vec4 v = cent + (p[i] - cent) * shrink;
    vec4 position = v + explode * vec4(normal, 0.0);
    
    gl_Position = projection * view * position;
 //gl_Position = projection * view * p[i];
    
    // pass through lighting calc vectors from vertex shader
    fN = gN[i];
    fL = gL[i];
    fV = gV[i];
    EmitVertex();
  }

  EndPrimitive();
}

#version 150

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 9) out;

uniform float shrink;
uniform float explode;
uniform float tesselate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat3 normalMatrix;
uniform vec4 lightPosition;

in vec3 gNormal[3];

out vec3 fN;
out vec3 fL;
out vec3 fV;

void createFace(vec3 a, vec3 b, vec3 c)
{
  vec3 N = normalize(cross(b - a, c - a));
  
    fN = (view * vec4(N, 0.0)).xyz;  
    fL = (view * lightPosition - view * vec4(a, 1.0)).xyz;

    fV = (-(view * vec4(a, 1.0)).xyz);
    gl_Position = projection * view * vec4(a, 1.0);
    EmitVertex();

    fN = (view * vec4(N, 0.0)).xyz;  
	fL = (view * lightPosition - view * vec4(b, 1.0)).xyz;
    fV = (-(view * vec4(b, 1.0)).xyz);
    gl_Position = projection * view * vec4(b, 1.0);
    EmitVertex();

    fN = (view * vec4(N, 0.0)).xyz;  
	fL = (view * lightPosition - view * vec4(c, 1.0)).xyz;
    fV = (-(view * vec4(c, 1.0)).xyz);
    gl_Position = projection * view * vec4(c, 1.0);
    EmitVertex();



  EndPrimitive();
}

void main() {
  // convert incoming values to world coordinates to get scale right
  vec4 p[3];
  p[0] = model * gl_in[0].gl_Position;
  p[1] = model * gl_in[1].gl_Position;
  p[2] = model * gl_in[2].gl_Position;
  
  vec3 a = p[0].xyz;
  vec3 b = p[1].xyz;
  vec3 c = p[2].xyz;
  
  vec3 normal = normalize(cross(b - a, c - a));
  vec4 cent = vec4(p[0] + p[1] + p[2]) / 3;
  vec3 d = (cent + tesselate * vec4(normal, 0.0)).xyz;
  
  createFace(a, b, d);
  createFace(b, c, d);
  createFace(c, a, d);
  
}

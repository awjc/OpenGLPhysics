#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec4 lightPosition;

in  vec4 vPosition;
in vec3 vNormal;

// pass to geometry shader
out vec3 gN;
out vec3 gL;
out vec3 gV;

void main() 
{
  gN = normalMatrix * vNormal;
  gL = (view * lightPosition - view * model * vPosition).xyz;
  gV = -(view * model * vPosition).xyz;
  
  gl_Position = projection * view * model * vPosition;

} 

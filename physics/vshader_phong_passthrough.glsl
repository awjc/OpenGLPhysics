#version 150

in  vec4 vPosition;
in vec3 vNormal;
out vec3 gNormal;

void main() 
{
  gNormal = vNormal;
  gl_Position = vPosition; 
} 

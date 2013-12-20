#version 150

uniform vec4 ambientLight;
uniform vec4 diffuseLight;
uniform vec4 specularLight;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;
//uniform sampler2D texture;

in vec3 fN;
in vec3 fL;
in vec3 fV;
//in  vec2 fTexCoord;

out vec4 color;

void main() 
{   
  vec4 diffuseSurface = diffuseColor;
  vec4 ambientSurface = ambientColor;
  vec4 specularSurface = specularColor;
  
  // have to normalize after interpolation
  vec3 N = normalize(fN);
  vec3 L = normalize(fL);
  vec3 V = normalize(fV);
  vec3 h = normalize(L + V);

  // ambient intensity
  //vec4 ia = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 ia = ambientLight * ambientSurface;
  
  // diffuse intensity
  float diffuseFactor = max(dot(L, N), 0.0);
  vec4 id = diffuseFactor * diffuseLight * diffuseSurface;
  
  // specular intensity
  vec4 is = vec4(0.0, 0.0, 0.0, 1.0);
  if (dot(L, N) >= 0)
  {
     is = pow(max(dot(N, h), 0.0), shininess) * specularLight * specularSurface;
  }
  
  color = ia + id + is;
  color.a = 1.0;
} 


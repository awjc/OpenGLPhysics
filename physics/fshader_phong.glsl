#version 150

uniform mat3 materialProperties;
uniform mat3 lightProperties;
uniform float shininess;

in vec3 fN;
in vec3 fL;
in vec3 fV;

out vec4 color;

void main() 
{ 
  // have to normalize after interpolation
  vec3 N = normalize(fN);
  vec3 L = normalize(fL);
  vec3 V = normalize(fV);
  
  
  // get rows from material and light properties
  mat3 material = transpose(materialProperties);
  mat3 light = transpose(lightProperties);
  vec4 ambientLight = vec4(light[0], 1.0);
  vec4 diffuseLight = vec4(light[1], 1.0);
  vec4 specularLight = vec4(light[2], 1.0);
  vec4 ambientSurface = vec4(material[0], 1.0);
  vec4 diffuseSurface = vec4(material[1], 1.0);
  vec4 specularSurface = vec4(material[2], 1.0);

  vec3 R = normalize(reflect(-L, N));
  //Or use: vec3 R = normalize(2 * (dot(L, N)) * N - L);
    
  // multiply material by light 
  vec4 ambientProduct = ambientLight * ambientSurface;
  vec4 diffuseProduct = diffuseLight * diffuseSurface;
  vec4 specularProduct = specularLight * specularSurface;

  // ambient intensity
  vec4 ia = ambientProduct;
  
  // diffuse intensity
  float diffuseFactor = max(dot(L, N), 0.0);
  vec4 id = diffuseFactor * diffuseProduct;
  
  // specular intensity
  vec4 is = vec4(0.0, 0.0, 0.0, 1.0);  
  if (dot(L, N) >= 0.0) 
  {
    float specularFactor = pow(max(dot(R, V), 0.0), shininess); 
    is = specularFactor * specularProduct;
  }
  
  color = ia + id + is;  
  color.a = 1.0;
} 


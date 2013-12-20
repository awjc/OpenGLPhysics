#version 150

uniform mat3 materialProperties;
uniform mat3 lightProperties;
uniform float shininess;
uniform bool black;

in vec3 fN;
in vec3 fL;
in vec3 fV;
out vec4 color;

void main() 
{ 
	int nSpecBins = 1;
	int nColBins = 4;
	float shinyScaled = shininess / 10;


	if(black) {
		color = vec4(0, 0, 0, 1.0);
		return;
	}

	vec4 kcool = vec4(.3, .3, 1.0, 1.0);
	vec4 kwarm = vec4(.8, .8, .2, 1.0);

  // have to normalize after interpolation
  vec3 N = normalize(fN);
  vec3 L = normalize(fL);
  vec3 V = normalize(fV);
  
  // Remaining code is directly cut-and-pasted from the vertex
  // shader vshader_gouraud.glsl for per-vertex lighting
  
  // Calculate reflected vector using built-in GLSL function, 
  // or use: vec3 R = normalize(2 * (dot(L, N)) * N - L);
  vec3 R = normalize(reflect(-L, N));
  
  // alternative calculation uses "half" vector
  vec3 h = normalize(L + V);
    
  // multiply material properties by light properties using "componentwise"
  // matrix multiplication, and then extract the rows
  // Rows are A, D, and S, and columns are R, G, and B components
  // Note that products[i] is ith *column* in GLSL, which is why we
  // have to transpose.
  // (This is just a shortcut to save having to do 9 separate multiplications.)
  mat3 products = transpose(matrixCompMult(lightProperties, materialProperties));
  vec4 ambientColor = vec4(products[0], 1);
  vec4 diffuseColor = vec4(products[1], 1);
  vec4 specularColor = vec4(products[2], 1);

  // ambient intensity
  vec4 ia = ambientColor;
  
  // diffuse intensity
  float diffuseFactor = max(dot(L, N), 0.0);
  vec4 id = diffuseFactor * diffuseColor;
  
  // specular intensity
  // This can be calculated with R dot V or using the alternative calculation 
  // N dot h.  This changes effect of the specular exponent.
  vec4 is = vec4(0.0, 0.0, 0.0, 1.0);  
  if (dot(L, N) >= 0.0) 
  {
    float specularFactor = pow(max(dot(R, V), 0.0), shinyScaled);
	specularFactor = floor(specularFactor*nSpecBins)/nSpecBins;
    is = specularFactor * specularColor;
  }

  kcool = ia;
  kwarm = diffuseColor;
  
  float scaleFactor = (1 + dot(N, L))/2;
  scaleFactor = floor(scaleFactor * nColBins)/nColBins;

  //color = ia + id + is;
  color = mix(kcool, kwarm, scaleFactor) + is;
  color.a = 1.0;
} 


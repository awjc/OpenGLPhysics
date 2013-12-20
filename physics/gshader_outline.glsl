#version 150

layout(triangles_adjacency) in;
layout(triangle_strip) out;
layout(max_vertices = 9) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat3 normalMatrix;
uniform vec4 lightPosition;

in vec3 gNormal[6];

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
	vec4 p[6];
	for(int i=0; i < 6; i++) {
		p[i] = model * gl_in[i].gl_Position;
	}

	vec3 a = p[0].xyz;
	vec3 b = p[1].xyz;
	vec3 c = p[2].xyz;
  
	vec4 normal1 = vec4(normalize(cross(p[1].xyz - p[0].xyz, p[2].xyz - p[0].xyz)), 0);
	vec4 normal2 = vec4(normalize(cross(p[4].xyz - p[3].xyz, p[5].xyz - p[3].xyz)), 0);

	if((projection * view * normal1).z * (projection * view * normal2).z < 0) {
		vec4 cent = vec4(p[0] + p[1] + p[2]) / 3;
		vec3 d = (cent + .2 * normal1).xyz;
		createFace(p[0].xyz, p[1].xyz, d);
		
	}

	//createFace(p[0].xyz, p[1].xyz, p[2].xyz);
	//createFace(p[3].xyz, p[4].xyz, p[5].xyz);
}

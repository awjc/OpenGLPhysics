#version 150
layout( triangles_adjacency ) in;
layout( line_strip, max_vertices=200 ) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color;

bool equals(vec3 a, vec3 b) {
	float tolerance = 0.000001;
	float dx = abs(a.x - b.x);
	float dy = abs(a.y - b.y);
	float dz = abs(a.z - b.z);

	return dx < tolerance && dy < tolerance && dz < tolerance;
}

void main()
{
	/*
	float f = sin(gl_in[0].gl_Position.x * gl_in[1].gl_Position.y) * cos(gl_in[2].gl_Position.z / (gl_in[4].gl_Position.x + .1));
	float g = cos(gl_in[0].gl_Position.x * gl_in[1].gl_Position.y) * sin(gl_in[1].gl_Position.z / (gl_in[5].gl_Position.x + .1));
	float h = sin(gl_in[0].gl_Position.x * gl_in[4].gl_Position.y) * sin(gl_in[1].gl_Position.z / (gl_in[2].gl_Position.x + .1));
	
	f = (f + 1) / 2;
	g = (g + 1) / 2;
	h = (h + 1) / 2;

	color = vec4(f, g, h, 1.0); //random
	*/


	//color = vec4(0, 1.0, 0, 1.0); //green
	color = vec4(0, 0.0, 0, 1.0); //black

	mat4 mm = projection * view * model;
	vec3 v[6];
	vec3 dv[6];
	for(int i=0; i < 6; i++) {
		v[i] = (mm * gl_in[i].gl_Position).xyz;//(mm * vec4(gl_in[i].gl_Position.xyz, 0.0)).xyz;
		dv[i] = gl_in[i].gl_Position.xyz;
	}

	vec3 N042 = cross( v[4]-v[0], v[2]-v[0] );
	vec3 N021 = cross( v[2]-v[0], v[1]-v[0] );
	vec3 N243 = cross( v[4]-v[2], v[3]-v[2] );
	vec3 N405 = cross( v[0]-v[4], v[5]-v[4] );

	if( N042.z * N021.z <= 0.0 )
	{
		gl_Position = (projection * view * model) * vec4( dv[0], 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dv[2], 1. );
		EmitVertex();
		EndPrimitive();
	}

	if( N042.z * N243.z <= 0.0 )
	{
		gl_Position = (projection * view * model) * vec4( dv[2], 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dv[4], 1. );
		EmitVertex();
		EndPrimitive();
	}

	if( N042.z * N405.z <= 0.0 )
	{
		gl_Position = (projection * view * model) * vec4( dv[4], 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dv[0], 1. );
		EmitVertex();
		EndPrimitive();
	}
}
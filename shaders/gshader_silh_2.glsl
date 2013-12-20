#version 150
layout( triangles_adjacency ) in;
layout( line_strip, max_vertices=200 ) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

bool isAdjacent(vec3 v0, vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 v5) {
	bool result = v0 == v3 || v0 == v4 || v0 == v5 || v1 == v3 || v1 == v4 || v1 == v5 || v2 == v3 || v2 == v4 || v2 == v5;
	return result;
}
bool IsFront(vec2 A, vec2 B, vec2 C)
{
    return 0 < (A.x * B.y - B.x * A.y) + (B.x * C.y - C.x * B.y) + (C.x * A.y - A.x * C.y);
}


void EmitEdge(vec2 P0, vec2 P1)
{
    vec2 V = normalize(P1 - P0);
   
    gl_Position = vec4(P0, 0, 1); EmitVertex();
  //  gl_Position = vec4(P0, 0, 1); EmitVertex();
    gl_Position = vec4(P1, 0, 1); EmitVertex();
  //  gl_Position = vec4(P1, 0, 1); EmitVertex();
    EndPrimitive();
}


void main()
{
	mat4 mm = view * model;

	vec2 V0 = (( mm) * vec4(gl_in[0].gl_Position.xyz, 0.0)).xy;
	vec2 V1 = (( mm) * vec4(gl_in[1].gl_Position.xyz, 0.0)).xy;
	vec2 V2 = (( mm) * vec4(gl_in[2].gl_Position.xyz, 0.0)).xy;
	vec2 V3 = (( mm) * vec4(gl_in[3].gl_Position.xyz, 0.0)).xy;
	vec2 V4 = (( mm) * vec4(gl_in[4].gl_Position.xyz, 0.0)).xy;
	vec2 V5 = (( mm) * vec4(gl_in[5].gl_Position.xyz, 0.0)).xy;

	vec3 dV0 = gl_in[0].gl_Position.xyz;
	vec3 dV1 = gl_in[1].gl_Position.xyz;
	vec3 dV2 = gl_in[2].gl_Position.xyz;
	vec3 dV3 = gl_in[3].gl_Position.xyz;
	vec3 dV4 = gl_in[4].gl_Position.xyz;
	vec3 dV5 = gl_in[5].gl_Position.xyz;

	vec3 N012 = cross( dV2-dV0, dV1-dV0 );
	vec3 N345 = cross( dV5-dV3, dV4-dV3 );

	//if(N012.z * N345.z <= 0) 
//	if(isAdjacent(dV0, dV1, dV2, dV3, dV4, dV5))
	if (IsFront(V0, V2, V4)) 
	{
		if (!IsFront(V0, V1, V2)) EmitEdge(V0, V2);
        if (!IsFront(V2, V3, V4)) EmitEdge(V2, V4);
        if (!IsFront(V0, V4, V5)) EmitEdge(V4, V0);
}
	if(false){
		gl_Position = (projection * view * model) * vec4( dV0, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV2, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV0, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV1, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV1, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV2, 1. );
		EmitVertex();

		gl_Position = (projection * view * model) * vec4( dV3, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV5, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV3, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV4, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV4, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( dV5, 1. );
		EmitVertex();
	
		EndPrimitive();
	}

	/*
	vec3 N042 = cross( V4-V0, V2-V0 );
	vec3 N021 = cross( V2-V0, V1-V0 );
	vec3 N243 = cross( V4-V2, V3-V2 );
	vec3 N405 = cross( V0-V4, V5-V4 );

	if( dot( N042, N021 ) < 0. )
		N021 = vec3(0.,0.,0.) - N021;
	if( dot( N042, N243 ) < 0. )
		N243 = vec3(0.,0.,0.) - N243;
	if( dot( N042, N405 ) < 0. )
		N405 = vec3(0.,0.,0.) - N405;

	if( N042.z * N021.z <= 0. )
	{
		gl_Position = (projection * view * model) * vec4( V0, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( V2, 1. );
		EmitVertex();
		EndPrimitive();
	}

	if( N042.z * N243.z <= 0. )
	{
		gl_Position = (projection * view * model) * vec4( V2, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( V4, 1. );
		EmitVertex();
		EndPrimitive();
	}

	if( N042.z * N405.z <= 0. )
	{
		gl_Position = (projection * view * model) * vec4( V4, 1. );
		EmitVertex();
		gl_Position = (projection * view * model) * vec4( V0, 1. );
		EmitVertex();
		EndPrimitive();
	}*/
}
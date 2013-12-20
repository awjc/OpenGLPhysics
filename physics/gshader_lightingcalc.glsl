#version 150

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec4 lightPosition;

in vec3 gNormal[3];

out vec3 fN;
out vec3 fL;
out vec3 fV;

void main() {
  
  for (int i = 0; i < 3; ++i)
  {
    vec4 position = gl_in[i].gl_Position;

    fN = normalMatrix * gNormal[i];
    fL = (view * lightPosition - view * model * position).xyz;
    fV = -(view * model * position).xyz;
    gl_Position = projection * view * model * position;
  


    //fN = ((view * model * vec4(gNormal[i], 0.0)).xyz);  
    //fV = (-(view * model * gl_in[i].gl_Position).xyz);
    //fL = ((view * lightVector).xyz) + fV;
    //gl_Position = projection * view * model * gl_in[i].gl_Position;
    EmitVertex();
  }

  EndPrimitive();
}

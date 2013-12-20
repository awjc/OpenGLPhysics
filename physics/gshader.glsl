#version 150

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform float shrink;

in vec3 gN[3];
in vec3 gL[3];
in vec3 gV[3];

out vec3 fN;
out vec3 fL;
out vec3 fV;

void main() {
  vec4 c = vec4(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
  
  for (int i = 0; i < 3; ++i)
  {
    // pass through
    gl_Position = c + (gl_in[i].gl_Position - c) * shrink;
    fN = gN[i];
    fL = gL[i];
    fV = gV[i];
    EmitVertex();
  }

  EndPrimitive();
}

#version 150

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

//layout(line_strip) out;
//layout(max_vertices = 4) out;



in vec3 gN[3];
in vec3 gL[3];
in vec3 gV[3];

out vec3 fN;
out vec3 fL;
out vec3 fV;

void main() {

  for (int i = 0; i < 3; ++i)
  {
    // pass through
    gl_Position = gl_in[i].gl_Position;
    fN = gN[i];
    fL = gL[i];
    fV = gV[i];
    EmitVertex();
  }

    // 4th vertex for line strip
    gl_Position = gl_in[0].gl_Position;
//    fN = gN[0];
//    fL = gL[0];
//    fV = gV[0];
//    EmitVertex();

  EndPrimitive();
}

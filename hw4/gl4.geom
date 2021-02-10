// Instancing geometry shader
// Takes a shape and instances it as a 3x3 cube of itself
#version 400 core
// Take in triangles, output triangle strips
layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 81) out;

// Input from vertex shader
in VS_GS_INTERFACE {
  in vec3 vNrm; // Normal
  in vec4 vCol; // Color
  in vec2 vTex; // TexCoord
  in mat4 vMVP; // ModelViewProjection
} gs_in[];

// Output to fragment shader
out GS_FS_INTERFACE {
  out vec3 vNrm;
  out vec4 vCol;
  out vec2 vTex;
} gs_out;

/*
 * Gives a vec4 point from a base
 * ex. if getting pos in 3^3, idx=5 and base=3, result is (2,1,0,1);
*/
vec4 idxToPoint(int idx, int base) {
  vec4 result = vec4(0.0,0.0,0.0,1.0);
  result.x = mod(idx,base);
  result.y = mod(floor(idx/base),base);
  result.z = floor(idx/pow(base,2));
  return result;
}

void main(void) {
  for(int i=0; i<27; i++) {
    vec4 mov = (idxToPoint(i, 3) - 1.0) * 3;
    // Loop over input vertices
    for(int n=0; n<gl_in.length(); n++) {
      vec4 pos   = gl_in[n].gl_Position + mov;
      // Copy over data for each vertex
      gl_Position = gs_in[n].vMVP * pos;
      //gl_Position.xyz = gl_Position.xyz + pos;
      gs_out.vNrm = gs_in[n].vNrm;
      gs_out.vCol = gs_in[n].vCol;
      gs_out.vTex = gs_in[n].vTex;
      // Then emit the vertex to add it to the output.
      EmitVertex();
    }
    // End the primitive. Not necessary, but looks nice.
    EndPrimitive();
  }
}
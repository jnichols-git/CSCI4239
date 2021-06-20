// Position vertices and assign texture coordinates
#version 120

uniform float tile;

void main() {
  // set vertex position (clip space);
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  // set texture coordinates
  vec4 vpos = (gl_Vertex+1)/2.0;
  gl_TexCoord[0] = vpos * tile;
}
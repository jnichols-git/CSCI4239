
varying vec2 tex; // texture coordinates to fragment shader

void main() {
  // set texture coordinates
  tex = gl_MultiTexCoord0.xy;
  // set vertex position
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
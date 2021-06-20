
varying vec4 ndc; // normalized device coordinates to fragment shader
varying vec2 tex; // texture coordinates to fragment shader

void main() {
  // set vertex position (clip space);
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  // get normalized device coordinates by dividing by C_w
  ndc = vec4(gl_Position.xyz/gl_Position.w, 1);
  // set texture coordinates
}
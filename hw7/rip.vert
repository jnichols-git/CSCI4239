// Re-assign texture coordinates
#version 400 core

// Attributes
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec2 Texture;

uniform vec3  pos;
float interval = 1.0/128.0;

// Out to GS
out VS_GS_INTERFACE {
  out vec4 vCol; // Color
  out vec2 vTex; // TexCoord
} vs_out;

void main() {
  vs_out.vCol = vec4(1.0); // Set to white
  vec2 corner = (Vertex.xz+1)/2.0; // (0,0), (1,0), (0,1), (1,1);
  vs_out.vTex = pos.xz + corner * interval;
  gl_Position = Vertex;
}